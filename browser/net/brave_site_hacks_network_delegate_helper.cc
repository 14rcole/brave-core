/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/net/brave_site_hacks_network_delegate_helper.h"

#include <map>
#include <string>
#include <vector>

#include "base/base64url.h"
#include "base/strings/string_util.h"
#include "brave/common/network_constants.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "brave/components/brave_shields/common/brave_shield_constants.h"
#include "components/subresource_filter/core/common/first_party_origin.h"
#include "extensions/common/url_pattern.h"
#include "net/url_request/url_request.h"

namespace brave {

bool IsEmptyDataURLRedirect(const GURL& gurl) {
  static std::vector<std::string> hosts({
    "sp1.nypost.com",
    "sp.nasdaq.com"
  });
  return std::find(hosts.begin(), hosts.end(), gurl.host()) !=
      hosts.end();
}

bool IsBlockedResource(const GURL& gurl) {
  static std::vector<URLPattern> blocked_patterns({
    URLPattern(URLPattern::SCHEME_ALL, "https://www.lesechos.fr/xtcore.js"),
    URLPattern(URLPattern::SCHEME_ALL, "https://*.y8.com/js/sdkloader/outstream.js")
  });
  return std::any_of(blocked_patterns.begin(), blocked_patterns.end(),
      [&gurl](URLPattern pattern){
        return pattern.MatchesURL(gurl);
      });
}

bool IsUAWhitelisted(const GURL& gurl) {
  static std::vector<URLPattern> whitelist_patterns({
    URLPattern(URLPattern::SCHEME_ALL, "https://*.adobe.com/*"),
    URLPattern(URLPattern::SCHEME_ALL, "https://*.duckduckgo.com/*"),
    URLPattern(URLPattern::SCHEME_ALL, "https://*.brave.com/*"),
    // For Widevine
    URLPattern(URLPattern::SCHEME_ALL, "https://*.netflix.com/*")
  });
  return std::any_of(whitelist_patterns.begin(), whitelist_patterns.end(),
      [&gurl](URLPattern pattern){
        return pattern.MatchesURL(gurl);
      });
}

bool IsWhitelistedReferer(const GURL& gurl) {
  static std::vector<URLPattern> whitelist_patterns({
    URLPattern(URLPattern::SCHEME_ALL, "https://use.typekit.net/*"),
    URLPattern(URLPattern::SCHEME_ALL, "https://cloud.typography.com/*"),
    URLPattern(URLPattern::SCHEME_ALL, "https://www.moremorewin.net/*")
  });
  return std::any_of(whitelist_patterns.begin(), whitelist_patterns.end(),
      [&gurl](URLPattern pattern){
        return pattern.MatchesURL(gurl);
      });
}

bool IsWhitelistedCookieExeption(const GURL& firstPartyOrigin,
    const GURL& subresourceUrl) {
  static std::map<GURL, std::vector<URLPattern> > whitelist_patterns = {{
      GURL("https://inbox.google.com"),
      { URLPattern(URLPattern::SCHEME_ALL, "https://hangouts.google.com/*") }
    }, {
      GURL("https://mail.google.com"),
      { URLPattern(URLPattern::SCHEME_ALL, "https://hangouts.google.com/*") }
    }, {
      GURL("https://drive.google.com"),
      { URLPattern(URLPattern::SCHEME_ALL,
          "https://doc-*-docs.googleusercontent.com/*") }
    }
  };
  std::map<GURL, std::vector<URLPattern> >::iterator i =
      whitelist_patterns.find(firstPartyOrigin);
  if (i == whitelist_patterns.end()) {
    return false;
  }
  std::vector<URLPattern> &exceptions = i->second;
  return std::any_of(exceptions.begin(), exceptions.end(),
      [&subresourceUrl](const URLPattern& pattern) {
        return pattern.MatchesURL(subresourceUrl);
      });
}

std::string GetGoogleTagManagerPolyfillJS() {
  static std::string base64_output;
  if (base64_output.length() != 0)  {
    return base64_output;
  }
  std::string str;
  str.reserve(135);
  str.append("(function() { var noopfn = function() { ; }; window.ga = window.ga || noopfn; })();");
  base64_output.reserve(180);
  Base64UrlEncode(str, base::Base64UrlEncodePolicy::OMIT_PADDING, &base64_output);
  base64_output = std::string(kJSDataURLPrefix) + base64_output;
  return base64_output;
}

std::string GetGoogleTagServicesPolyfillJS() {
  static std::string base64_output;
  if (base64_output.length() != 0)  {
    return base64_output;
  }
  std::string str;
  str.reserve(3500);
  str.append("(function() { var p; var noopfn = function() { }; var noopthisfn = function() { return this; }; var noopnullfn = function() { return null; }; var nooparrayfn = function() { return []; }; var noopstrfn = function() { return ''; }; var companionAdsService = { addEventListener: noopthisfn, enableSyncLoading: noopfn, setRefreshUnfilledSlots: noopfn }; var contentService = { addEventListener: noopthisfn, setContent: noopfn }; var PassbackSlot = function() { }; p = PassbackSlot.prototype; p.display =");
  str.append(" noopfn; p.get = noopnullfn; p.set = noopthisfn; p.setClickUrl = noopthisfn; p.setTagForChildDirectedTreatment = noopthisfn; p.setTargeting = noopthisfn; p.updateTargetingFromMap = noopthisfn; var pubAdsService = { addEventListener: noopthisfn, clear: noopfn, clearCategoryExclusions: noopthisfn, clearTagForChildDirectedTreatment: noopthisfn, clearTargeting: noopthisfn, collapseEmptyDivs: noopfn, defineOutOfPagePassback: function() { return new PassbackSlot(); }, definePassback: function() { ret");
  str.append("urn new PassbackSlot(); }, disableInitialLoad: noopfn, display: noopfn, enableAsyncRendering: noopfn, enableSingleRequest: noopfn, enableSyncRendering: noopfn, enableVideoAds: noopfn, get: noopnullfn, getAttributeKeys: nooparrayfn, refresh: noopfn, set: noopthisfn, setCategoryExclusion: noopthisfn, setCentering: noopfn, setCookieOptions: noopthisfn, setLocation: noopthisfn, setPublisherProvidedId: noopthisfn, setTagForChildDirectedTreatment: noopthisfn, setTargeting: noopthisfn, setVideoContent");
  str.append(": noopthisfn, updateCorrelator: noopfn }; var SizeMappingBuilder = function() { }; p = SizeMappingBuilder.prototype; p.addSize = noopthisfn; p.build = noopnullfn; var Slot = function() { }; p = Slot.prototype; p.addService = noopthisfn; p.clearCategoryExclusions = noopthisfn; p.clearTargeting = noopthisfn; p.defineSizeMapping = noopthisfn; p.get = noopnullfn; p.getAdUnitPath = nooparrayfn; p.getAttributeKeys = nooparrayfn; p.getCategoryExclusions = nooparrayfn; p.getDomId = noopstrfn; p.getSlot");
  str.append("ElementId = noopstrfn; p.getSlotId = noopthisfn; p.getTargeting = nooparrayfn; p.getTargetingKeys = nooparrayfn; p.set = noopthisfn; p.setCategoryExclusion = noopthisfn; p.setClickUrl = noopthisfn; p.setCollapseEmptyDiv = noopthisfn; p.setTargeting = noopthisfn; var gpt = window.googletag || {}; window.googletag.destroySlots = function () { }; var cmd = gpt.cmd || []; gpt.apiReady = true; gpt.cmd = []; gpt.cmd.push = function(a) { try { a(); } catch (ex) { } return 1; }; gpt.companionAds = func");
  str.append("tion() { return companionAdsService; }; gpt.content = function() { return contentService; }; gpt.defineOutOfPageSlot = function() { return new Slot(); }; gpt.defineSlot = function() { return new Slot(); }; gpt.disablePublisherConsole = noopfn; gpt.display = noopfn; gpt.enableServices = noopfn; gpt.getVersion = noopstrfn; gpt.pubads = function() { return pubAdsService; }; gpt.pubadsReady = true; gpt.sizeMapping = function() { return new SizeMappingBuilder(); }; window.googletag = gpt; while ( cm");
  str.append("d.length !== 0 ) { gpt.cmd.push(cmd.shift()); } })();");
  base64_output.reserve(4668);
  Base64UrlEncode(str, base::Base64UrlEncodePolicy::OMIT_PADDING, &base64_output);
  base64_output = std::string(kJSDataURLPrefix) + base64_output;
  return base64_output;
}

bool GetPolyfill(const GURL& gurl, GURL *new_url) {
  static URLPattern tag_manager(URLPattern::SCHEME_ALL, kGoogleTagManagerPattern);
  static URLPattern tag_services(URLPattern::SCHEME_ALL, kGoogleTagServicesPattern);
  if (tag_manager.MatchesURL(gurl)) {
    std::string&& data_url = GetGoogleTagManagerPolyfillJS();
    *new_url = GURL(data_url);
  } else if (tag_services.MatchesURL(gurl)) {
    std::string&& data_url = GetGoogleTagServicesPolyfillJS();
    *new_url = GURL(data_url);
  }
  return net::OK;
}

int OnBeforeURLRequest_SiteHacksWork(
    net::URLRequest* request,
    GURL* new_url,
    const ResponseCallback& next_callback,
    std::shared_ptr<BraveRequestInfo> ctx) {
  const GURL& url = request->url();

  if (IsEmptyDataURLRedirect(url)) {
    *new_url = GURL(kEmptyDataURI);
    return net::OK;
  }

  if (IsBlockedResource(url)) {
    request->Cancel();
    return net::ERR_ABORTED;
  }

  if (GetPolyfill(url, new_url)) {
    return net::OK;
  }

  return net::OK;
}

void CheckForCookieOverride(const GURL& url, const URLPattern& pattern,
    net::HttpRequestHeaders* headers, const std::string& extra_cookies) {
  if (pattern.MatchesURL(url)) {
    std::string cookies;
    if (headers->GetHeader(kCookieHeader, &cookies)) {
      cookies = "; ";
    }
    cookies += extra_cookies;
    headers->SetHeader(kCookieHeader, cookies);
  }
}

bool IsBlockTwitterSiteHack(net::URLRequest* request,
    net::HttpRequestHeaders* headers) {
  URLPattern redirectURLPattern(URLPattern::SCHEME_ALL, kTwitterRedirectURL);
  URLPattern referrerPattern(URLPattern::SCHEME_ALL, kTwitterReferrer);
  if (redirectURLPattern.MatchesURL(request->url())) {
    std::string referrer;
    if (headers->GetHeader(kRefererHeader, &referrer) &&
        referrerPattern.MatchesURL(GURL(referrer))) {
      return true;
    }
  }
  return false;
}

bool ApplyPotentialRefererBlock(net::URLRequest* request,
    net::HttpRequestHeaders* headers) {
  GURL target_origin = GURL(request->url()).GetOrigin();
  bool allow_referers = brave_shields::IsAllowContentSettingFromIO(
      request, target_origin, target_origin, CONTENT_SETTINGS_TYPE_PLUGINS,
      brave_shields::kReferers);
  std::string referrer;
  if (headers->GetHeader(kRefererHeader, &referrer) &&
      !IsWhitelistedReferer(request->url()) &&
      !allow_referers) {
    GURL referrer_origin = GURL(referrer).GetOrigin();
    if (referrer_origin != target_origin) {
      headers->SetHeader(kRefererHeader, target_origin.spec());
      return true;
    }
  }
  return false;
}

bool ApplyPotentialCookieBlock(net::URLRequest* request,
    net::HttpRequestHeaders* headers) {
  GURL target_origin = GURL(request->url()).GetOrigin();
  bool allow_cookies = brave_shields::IsAllowContentSettingFromIO(
      request, target_origin, GURL(), CONTENT_SETTINGS_TYPE_COOKIES, "");
  bool allow_1p_cookies = brave_shields::IsAllowContentSettingFromIO(
      request, target_origin, target_origin, CONTENT_SETTINGS_TYPE_COOKIES, "");


  GURL firstPartyForCookiesOrigin(request->site_for_cookies().GetOrigin());
  bool is_first_party =
      subresource_filter::FirstPartyOrigin::IsThirdParty(request->url(),
          url::Origin::Create(firstPartyForCookiesOrigin));

  bool block_cookies =
      !allow_1p_cookies ||
      (!allow_cookies && allow_1p_cookies && !is_first_party);

  std::string cookies;
  if (headers->GetHeader(kCookieHeader, &cookies) &&
      !request->site_for_cookies().is_empty() &&
      !IsWhitelistedCookieExeption(firstPartyForCookiesOrigin, request->url()) &&
      block_cookies) {
    headers->RemoveHeader(kCookieHeader);
  }
  return false;
}

int OnBeforeStartTransaction_SiteHacksWork(net::URLRequest* request,
        net::HttpRequestHeaders* headers,
        const ResponseCallback& next_callback,
        std::shared_ptr<BraveRequestInfo> ctx) {
  CheckForCookieOverride(request->url(),
      URLPattern(URLPattern::SCHEME_ALL, kForbesPattern), headers,
      kForbesExtraCookies);
  if (IsBlockTwitterSiteHack(request, headers)) {
    request->Cancel();
    return net::ERR_ABORTED;
  }
  if (IsUAWhitelisted(request->url())) {
    std::string user_agent;
    if (headers->GetHeader(kUserAgentHeader, &user_agent)) {
      base::ReplaceFirstSubstringAfterOffset(&user_agent, 0, "Chrome", "Brave Chrome");
      headers->SetHeader(kUserAgentHeader, user_agent);
    }
  }
  ApplyPotentialRefererBlock(request, headers);
  ApplyPotentialCookieBlock(request, headers);
  return net::OK;
}

}  // namespace brave
