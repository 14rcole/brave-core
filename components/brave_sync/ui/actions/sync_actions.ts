/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { action } from 'typesafe-actions'

// Constants
import { types } from '../constants/sync_types'

export const removeDevice = (deviceId: number) => action(types.SYNC_REMOVE_DEVICE, { deviceId })

export const syncReset = () => action(types.SYNC_RESET)

export const enableSync = () => action(types.SYNC_ENABLE)

export const toggleDeviceSyncing = (setImmediateSyncDevice: boolean) => action(types.SYNC_TOGGLE_DEVICE_SYNCING, { setImmediateSyncDevice })

export const setDeviceName = (deviceName: string) => action(types.SYNC_SET_DEVICE_NAME, { deviceName })

export const enableBookmarksSync = (shouldEnable: boolean) => action(types.SYNC_BOOKMARKS_DATA, { shouldEnable })

export const enableSavedSiteSettingsSync = (shouldEnable: boolean) => action(types.SYNC_SAVED_SITE_SETTINGS, { shouldEnable })

export const enableBrowsingHistorySync = (shouldEnable: boolean) => action(types.SYNC_BROWSING_HISTORY, { shouldEnable })

// TODO: CHECK

/**
 * Dispatches a message when sync init data needs to be saved
 * @param {Array.<number>|null} seed
 */
// export const saveSyncInitData = (/*seed: any*/) => {
//   TODO
// }

/**
 * Dispatches a message when sync needs to be restarted
 * @param {Array.<number>|null} seed
 */
// export const reloadSyncExtension = () => {
  // TODO
// }

/**
 * Dispatches a message to reset Sync data on this device and the cloud.
 */
// export const resetSync = () => {
  // TODO
// }
