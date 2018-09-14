#ifndef BRAVE_BROWSER_RENDERER_HOST_BRAVE_RENDER_MESSAGE_FILTER_H_
#define BRAVE_BROWSER_RENDERER_HOST_BRAVE_RENDER_MESSAGE_FILTER_H_

#include <string>
#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "base/sequenced_task_runner_helpers.h"
#include "chrome/browser/renderer_host/chrome_render_message_filter.h"
#include "content/public/browser/browser_message_filter.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "extensions/buildflags/buildflags.h"
#include "ppapi/buildflags/buildflags.h"

class GURL;
class Profile;

namespace predictors {
class PreconnectManager;
}

namespace content_settings {
class CookieSettings;
}

namespace network_hints {
struct LookupRequest;
}

class BraveRenderMessageFilter : public ChromeRenderMessageFilter {
 public:
  BraveRenderMessageFilter(int render_process_id, Profile* profile);
  bool OnMessageReceived(const IPC::Message& message) override;
  //bool FirstPartyTracker(content::WebContents* contents, const GURL& top_origin_url, 
  //const GURL& origin_url);
  
 private:
  friend class content::BrowserThread;
  friend class base::DeleteHelper<ChromeRenderMessageFilter>; 
  friend class base::DeleteHelper<BraveRenderMessageFilter>; 
  friend class ChromeRenderMessageFilter;

  ~BraveRenderMessageFilter() override;

  void OnAllowDatabase(int render_frame_id,
                       const GURL& origin_url,
                       const GURL& top_origin_url,
                       const base::string16& name,
                       const base::string16& display_name,
                       bool* allowed);

  void OnAllowDatabaseInternal(int render_frame_id,
               const GURL& origin_url,
               const GURL& top_origin_url,
               const base::string16& name,
               const base::string16& display_name,
               bool* allowed);

  void OnAllowDOMStorage(int render_frame_id,
                         const GURL& origin_url,
                         const GURL& top_origin_url,
                         bool local,
                         bool* allowed);

  void OnAllowDOMStorageInternal(int render_frame_id,
                         const GURL& origin_url,
                         const GURL& top_origin_url,
                         bool local,
                         bool* allowed);

  void OnAllowIndexedDB(int render_frame_id,
                        const GURL& origin_url,
                        const GURL& top_origin_url,
                        const base::string16& name,
                        bool* allowed);

  void OnAllowIndexedDBInternal(int render_frame_id,
                        const GURL& origin_url,
                        const GURL& top_origin_url,
                        const base::string16& name,
                        bool* allowed);

  void ShouldStoreState(int render_process_id, int render_frame_id, 
    const GURL& origin_url, const GURL& top_origin_url, bool *allowed);
  
  /*void OnRequestFileSystemAccessSync(int render_frame_id,
                                     const GURL& origin_url,
                                     const GURL& top_origin_url,
                                     IPC::Message* message);
  void OnRequestFileSystemAccessAsync(int render_frame_id,
                                      int request_id,
                                      const GURL& origin_url,
                                      const GURL& top_origin_url);
  void OnRequestFileSystemAccess(int render_frame_id,
                                 const GURL& origin_url,
                                 const GURL& top_origin_url,
                                 base::Callback<void(bool)> callback);*/
  
  base::WeakPtrFactory<BraveRenderMessageFilter> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(BraveRenderMessageFilter);
};

#endif  // BRAVE_BROWSER_RENDERER_HOST_BRAVE_RENDER_MESSAGE_FILTER_H_