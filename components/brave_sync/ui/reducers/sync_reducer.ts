/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { Reducer } from 'redux'

// Constants
import { types } from '../constants/sync_types'

// Utils
import * as storage from '../storage'

const syncReducer: Reducer<Sync.State | undefined> = (state: Sync.State | undefined, action: any) => {
  if (state === undefined) {
    state = storage.load()
  }

  const payload = action.payload
  const startingState = state

  switch (action.type) {
    case types.SYNC_REMOVE_DEVICE:
      // chrome.send('deleteDevice', action.deviceId)
      break
    // chrome.send('needSyncQRcode')
    // chrome.send('needSyncWords')
    case types.SYNC_ENABLE:
      state = { ...state, isSyncEnabled: true }
      break
    case types.SYNC_RESET:
      // chrome.send('resetSync')
      state = { ...state, isSyncEnabled: false }
      break
    case types.SYNC_SET_DEVICE_NAME:
      state = { ...state, deviceName: payload.deviceName }
      break
    case types.SYNC_TOGGLE_DEVICE_SYNCING:
      state = { ...state, setImmediateSyncDevice: payload.setImmediateSyncDevice }
      break
    case types.SYNC_BOOKMARKS_DATA:
      state = { ...state, syncBookmarks: payload.shouldEnable }
      break
    case types.SYNC_SAVED_SITE_SETTINGS:
      state = { ...state, syncSavedSiteSettings: payload.shouldEnable }
      break
    case types.SYNC_BROWSING_HISTORY:
      state = { ...state, syncBrowsingHistory: payload.shouldEnable }
      break
  }

  if (state !== startingState) {
    storage.debouncedSave(state)
  }

  return state
}

export default syncReducer
