declare namespace Sync {
  export interface ApplicationState {
    syncData: State | undefined
  }

  export interface State {
    isSyncEnabled: boolean
    deviceName: string
    setImmediateSyncDevice: boolean
    syncBookmarks: boolean,
    syncSavedSiteSettings: boolean,
    syncBrowsingHistory: boolean
  }
}
