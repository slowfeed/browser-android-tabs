// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_path.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "base/test/scoped_feature_list.h"
#include "base/threading/thread_restrictions.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/common/content_features.h"
#include "content/public/common/page_type.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/content_browser_test.h"
#include "content/public/test/content_browser_test_utils.h"
#include "content/public/test/test_navigation_throttle.h"
#include "content/public/test/url_loader_interceptor.h"
#include "content/shell/browser/shell.h"
#include "net/test/test_data_directory.h"
#include "net/test/url_request/url_request_mock_http_job.h"
#include "net/url_request/url_request_filter.h"
#include "net/url_request/url_request_interceptor.h"
#include "services/network/public/cpp/features.h"

namespace content {

namespace {

const char* kMockHeaderFileSuffix = ".mock-http-headers";

class NavigationFailureObserver : public WebContentsObserver {
 public:
  explicit NavigationFailureObserver(WebContents* web_contents)
      : WebContentsObserver(web_contents) {}
  ~NavigationFailureObserver() override = default;

  void DidStartNavigation(content::NavigationHandle* handle) override {
    auto throttle = std::make_unique<TestNavigationThrottle>(handle);
    throttle->SetCallback(
        TestNavigationThrottle::WILL_FAIL_REQUEST,
        base::BindRepeating(&NavigationFailureObserver::OnWillFailRequestCalled,
                            base::Unretained(this)));
    handle->RegisterThrottleForTesting(
        std::unique_ptr<TestNavigationThrottle>(std::move(throttle)));
  }

  bool did_fail() const { return did_fail_; }

 private:
  void OnWillFailRequestCalled() { did_fail_ = true; }

  bool did_fail_ = false;

  DISALLOW_COPY_AND_ASSIGN(NavigationFailureObserver);
};

}  // namespace

class WebPackageRequestHandlerBrowserTest
    : public ContentBrowserTest,
      public testing::WithParamInterface<bool> {
 public:
  WebPackageRequestHandlerBrowserTest() = default;
  ~WebPackageRequestHandlerBrowserTest() = default;

  void SetUp() override {
    if (is_network_service_enabled()) {
      feature_list_.InitWithFeatures(
          {features::kSignedHTTPExchange, network::features::kNetworkService},
          {});
    } else {
      feature_list_.InitWithFeatures({features::kSignedHTTPExchange}, {});
    }
    ContentBrowserTest::SetUp();
  }

  void TearDownOnMainThread() override { interceptor_.reset(); }

 protected:
  void InstallUrlInterceptor(const GURL& url, const std::string& data_path) {
    if (base::FeatureList::IsEnabled(network::features::kNetworkService)) {
      if (!interceptor_) {
        interceptor_ =
            std::make_unique<content::URLLoaderInterceptor>(base::BindRepeating(
                &WebPackageRequestHandlerBrowserTest::OnInterceptCallback,
                base::Unretained(this)));
      }
      interceptor_data_path_map_[url] = data_path;
    } else {
      BrowserThread::PostTask(
          BrowserThread::IO, FROM_HERE,
          base::BindOnce(&InstallMockInterceptors, url, data_path));
    }
  }

 private:
  static std::string ReadFile(const std::string& data_path) {
    base::ScopedAllowBlockingForTesting allow_io;
    base::FilePath root_path;
    CHECK(PathService::Get(base::DIR_SOURCE_ROOT, &root_path));
    std::string contents;
    CHECK(base::ReadFileToString(root_path.AppendASCII(data_path), &contents));
    return contents;
  }

  static std::string ReadHeaderFile(const std::string& data_path) {
    std::string header_file_relative_path = data_path + kMockHeaderFileSuffix;
    base::ScopedAllowBlockingForTesting allow_io;
    base::FilePath root_path;
    CHECK(PathService::Get(base::DIR_SOURCE_ROOT, &root_path));
    if (!base::PathExists(root_path.AppendASCII(header_file_relative_path)))
      return "HTTP/1.0 200 OK\n";
    return ReadFile(header_file_relative_path);
  }

  static void InstallMockInterceptors(const GURL& url,
                                      const std::string& data_path) {
    DCHECK(!base::FeatureList::IsEnabled(network::features::kNetworkService));
    base::FilePath root_path;
    CHECK(PathService::Get(base::DIR_SOURCE_ROOT, &root_path));
    net::URLRequestFilter::GetInstance()->AddUrlInterceptor(
        url, net::URLRequestMockHTTPJob::CreateInterceptorForSingleFile(
                 root_path.AppendASCII(data_path)));
  }

  bool OnInterceptCallback(
      content::URLLoaderInterceptor::RequestParams* params) {
    DCHECK(base::FeatureList::IsEnabled(network::features::kNetworkService));
    const auto it = interceptor_data_path_map_.find(params->url_request.url);
    if (it == interceptor_data_path_map_.end())
      return false;
    content::URLLoaderInterceptor::WriteResponse(
        ReadHeaderFile(it->second), ReadFile(it->second), params->client.get());
    return true;
  }

  bool is_network_service_enabled() const { return GetParam(); }

  base::test::ScopedFeatureList feature_list_;

  std::unique_ptr<content::URLLoaderInterceptor> interceptor_;
  std::map<GURL, std::string> interceptor_data_path_map_;

  DISALLOW_COPY_AND_ASSIGN(WebPackageRequestHandlerBrowserTest);
};

IN_PROC_BROWSER_TEST_P(WebPackageRequestHandlerBrowserTest, Simple) {
  InstallUrlInterceptor(
      GURL("https://cert.example.org/cert.msg"),
      "content/test/data/htxg/wildcard_example.org.public.pem.msg");

  embedded_test_server()->ServeFilesFromSourceDirectory("content/test/data");
  ASSERT_TRUE(embedded_test_server()->Start());
  GURL url = embedded_test_server()->GetURL("/htxg/test.example.org_test.htxg");
  base::string16 title = base::ASCIIToUTF16("https://test.example.org/test/");
  TitleWatcher title_watcher(shell()->web_contents(), title);
  NavigateToURL(shell(), url);
  EXPECT_EQ(title, title_watcher.WaitAndGetTitle());
}

IN_PROC_BROWSER_TEST_P(WebPackageRequestHandlerBrowserTest, CertNotFound) {
  InstallUrlInterceptor(GURL("https://cert.example.org/cert.msg"),
                        "content/test/data/htxg/404.msg");

  embedded_test_server()->ServeFilesFromSourceDirectory("content/test/data");
  ASSERT_TRUE(embedded_test_server()->Start());
  GURL url = embedded_test_server()->GetURL("/htxg/test.example.org_test.htxg");

  NavigationFailureObserver failure_observer(shell()->web_contents());
  NavigateToURL(shell(), url);
  EXPECT_TRUE(failure_observer.did_fail());
  NavigationEntry* entry =
      shell()->web_contents()->GetController().GetVisibleEntry();
  EXPECT_EQ(content::PAGE_TYPE_ERROR, entry->GetPageType());
}

INSTANTIATE_TEST_CASE_P(WebPackageRequestHandlerBrowserTest,
                        WebPackageRequestHandlerBrowserTest,
                        testing::Bool());

}  // namespace content
