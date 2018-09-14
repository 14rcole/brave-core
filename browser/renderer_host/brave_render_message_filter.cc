/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/renderer_host/brave_render_message_filter.h"

#include "base/sequenced_task_runner.h"
#include "brave/browser/brave_browser_process_impl.h"
#include "brave/components/brave_shields/browser/tracking_protection_service.h"
#include "brave/components/brave_shields/browser/brave_shields_util.h"
#include "brave/components/brave_shields/common/brave_shield_constants.h"
#include "components/content_settings/core/browser/cookie_settings.h"
#include "content/public/browser/browser_thread.h"
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "chrome/browser/content_settings/tab_specific_content_settings.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/render_messages.h"
#include "content/public/browser/web_contents.h"
#include "content/browser/web_contents/web_contents_impl.h"

using content::BrowserThread;
using content::RenderFrameHost;
using content::WebContentsImpl;

BraveRenderMessageFilter::BraveRenderMessageFilter(int render_process_id, 
	Profile* profile)
	: ChromeRenderMessageFilter(render_process_id, profile),
	  weak_factory_(this) {

}

BraveRenderMessageFilter::~BraveRenderMessageFilter() {}

bool BraveRenderMessageFilter::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(BraveRenderMessageFilter, message)
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowDatabase, OnAllowDatabase);
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowDOMStorage, OnAllowDOMStorage);
    IPC_MESSAGE_HANDLER(ChromeViewHostMsg_AllowIndexedDB, OnAllowIndexedDB);
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()

  if (handled) return true; 
  return ChromeRenderMessageFilter::OnMessageReceived(message);
}

void BraveRenderMessageFilter::ShouldStoreState(int render_process_id, int render_frame_id, 
	const GURL& origin_url, const GURL& top_origin_url, bool *allowed) {
	DCHECK_CURRENTLY_ON(BrowserThread::UI);
	CHECK(g_brave_browser_process->tracking_protection_service()->IsInitialized());

	auto *contents = WebContentsImpl::FromRenderFrameHostID(render_process_id, render_frame_id);

	HostContentSettingsMap* map = HostContentSettingsMapFactory::GetForProfile(
      Profile::FromBrowserContext(contents->GetBrowserContext()));

	bool allow_brave_shields = brave_shields::IsAllowContentSetting(
      map, top_origin_url, top_origin_url, CONTENT_SETTINGS_TYPE_PLUGINS,
      brave_shields::kBraveShields);

	bool allow_trackers = brave_shields::IsAllowContentSetting(
      map, top_origin_url, top_origin_url,
      CONTENT_SETTINGS_TYPE_PLUGINS, brave_shields::kTrackers);

  	if (allow_brave_shields && !allow_trackers &&
      !g_brave_browser_process->tracking_protection_service()->
      ShouldStoreState(origin_url)) {
      	*allowed = false;
  	}
  	*allowed = true;
}

void BraveRenderMessageFilter::OnAllowDatabaseInternal(int render_frame_id,
               const GURL& origin_url,
               const GURL& top_origin_url,
               const base::string16& name,
               const base::string16& display_name,
               bool* allowed) {
	*allowed =
      *allowed && cookie_settings_->IsCookieAccessAllowed(origin_url, top_origin_url);
  	BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&TabSpecificContentSettings::WebDatabaseAccessed,
                 render_process_id_, render_frame_id, origin_url, name,
                 display_name, !*allowed));
}

void BraveRenderMessageFilter::OnAllowDatabase(int render_frame_id,
               const GURL& origin_url,
               const GURL& top_origin_url,
               const base::string16& name,
               const base::string16& display_name,
               bool* allowed) {
	BrowserThread::PostTaskAndReply(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&BraveRenderMessageFilter::ShouldStoreState, base::Unretained(this), 
      	render_process_id_, render_frame_id, origin_url, top_origin_url, allowed), 
      base::Bind(&BraveRenderMessageFilter::OnAllowDatabaseInternal, base::Unretained(this), 
      	render_frame_id, origin_url, top_origin_url, name, display_name, allowed));	
}

void BraveRenderMessageFilter::OnAllowDOMStorageInternal(int render_frame_id,
               const GURL& origin_url,
               const GURL& top_origin_url,
               bool local,
               bool* allowed) {
  *allowed =
      cookie_settings_->IsCookieAccessAllowed(origin_url, top_origin_url);
  // Record access to DOM storage for potential display in UI.
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&TabSpecificContentSettings::DOMStorageAccessed,
                 render_process_id_, render_frame_id, origin_url, local,
                 !*allowed));
  
}

void BraveRenderMessageFilter::OnAllowDOMStorage(int render_frame_id,
                 const GURL& origin_url,
                 const GURL& top_origin_url,
                 bool local,
                 bool* allowed) {
	BrowserThread::PostTaskAndReply(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&BraveRenderMessageFilter::ShouldStoreState, base::Unretained(this), 
      	render_process_id_, render_frame_id, origin_url, top_origin_url, allowed), 
      base::Bind(&BraveRenderMessageFilter::OnAllowDOMStorageInternal, base::Unretained(this), 
      	render_frame_id, origin_url, top_origin_url, local, allowed));
}

void BraveRenderMessageFilter::OnAllowIndexedDBInternal(int render_frame_id,
                const GURL& origin_url,
                const GURL& top_origin_url,
                const base::string16& name,
                bool* allowed) {
  *allowed = allowed &&
      cookie_settings_->IsCookieAccessAllowed(origin_url, top_origin_url);
  BrowserThread::PostTask(
      BrowserThread::UI, FROM_HERE,
      base::Bind(&TabSpecificContentSettings::IndexedDBAccessed,
                 render_process_id_, render_frame_id, origin_url, name,
                 !*allowed));
}

void BraveRenderMessageFilter::OnAllowIndexedDB(int render_frame_id,
                const GURL& origin_url,
                const GURL& top_origin_url,
                const base::string16& name,
                bool* allowed) {
	BrowserThread::PostTaskAndReply(
	  BrowserThread::UI, FROM_HERE,
	  base::Bind(&BraveRenderMessageFilter::ShouldStoreState, base::Unretained(this), 
	  	render_process_id_, render_frame_id, origin_url, top_origin_url, allowed), 
	  base::Bind(&BraveRenderMessageFilter::OnAllowIndexedDBInternal, base::Unretained(this), 
	  	render_frame_id, origin_url, top_origin_url, name, allowed));
}

/*
void OnRequestFileSystemAccessSync(int render_frame_id,
                             const GURL& origin_url,
                             const GURL& top_origin_url,
                             IPC::Message* message);
void OnRequestFileSystemAccessAsync(int render_frame_id,
                              int request_id,
                              const GURL& origin_url,
                              const GURL& top_origin_url);
void OnRequestFileSystemAccessSyncResponse(IPC::Message* reply_msg,
                                     bool allowed);
void OnRequestFileSystemAccessAsyncResponse(int render_frame_id,
                                      int request_id,
                                      bool allowed);
void OnRequestFileSystemAccess(int render_frame_id,
                         const GURL& origin_url,
                         const GURL& top_origin_url,
                         base::Callback<void(bool)> callback);
*/