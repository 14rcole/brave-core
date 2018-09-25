/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'

// Components
import { Card, Button } from 'brave-ui'
import SwitchButton from 'brave-ui/old/switchButton'
import Table, { Cell, Row } from 'brave-ui/components/dataTables/table'

// Feature-specific components
import {
  Grid,
  FlexColumn,
  SwitchLabel,
  Label,
  Paragraph,
  SectionBlock,
  SubTitle
} from 'brave-ui/features/sync'

// Modals
import SyncANewDeviceModal from './modals/syncNewDevice'
import ResetSyncModal from './modals/resetSync'

// Utils
import { getLocale } from '../../../common/locale'

interface SyncEnabledContentProps {
  syncData: Sync.State
  actions: any
}

interface SyncEnabledContentState {
  syncANewDevice: boolean
  resetSync: boolean
}

class SyncEnabledContent extends React.PureComponent<SyncEnabledContentProps, SyncEnabledContentState> {
  constructor (props: SyncEnabledContentProps) {
    super(props)
    this.state = {
      syncANewDevice: false,
      resetSync: false
    }
  }

  get rows (): Row[] {
    return [
      {
        content: [
          { content: 1 },
          { content: 'MacOS without the ESC key' },
          { content: '6/12/2018, 12:10:16 PM' }
        ]
      },
      {
        content: [
          { content: 2 },
          { content: 'Windowz machineh' },
          { content: '8/1/2018, 7:12:32 PM' }
        ]
      }
    ]
  }

  get header (): Cell[] {
    return [
      { content: getLocale('id') },
      { content: getLocale('deviceName') },
      { content: getLocale('lastActive') }
    ]
  }

  toggleDeviceSyncing = (event: React.ChangeEvent<HTMLInputElement>) => {
    const { actions } = this.props
    if (event.target) {
      actions.toggleDeviceSyncing(event.target.checked)
    }
  }

  syncANewDevice = () => {
    this.setState({ syncANewDevice: !this.state.syncANewDevice })
  }

  resetSync = () => {
    this.setState({ resetSync: !this.state.resetSync })
  }

  onChangeSyncBookmarks = (event: React.ChangeEvent<HTMLInputElement>) => {
    const { actions } = this.props
    if (event.target) {
      actions.enableBookmarksSync(event.target.checked)
    }
  }
  onChangeSavedSiteSettings = (event: React.ChangeEvent<HTMLInputElement>) => {
    const { actions } = this.props
    if (event.target) {
      actions.enableSavedSiteSettingsSync(event.target.checked)
    }
  }
  onChangeBrowsingHistory = (event: React.ChangeEvent<HTMLInputElement>) => {
    const { actions } = this.props
    if (event.target) {
      actions.enableBrowsingHistorySync(event.target.checked)
    }
  }

  render () {
    const { actions, syncData } = this.props
    console.log('hahaahahahahahahahaha', syncData.syncBookmarks)
    return (
      <>
        {
          this.state.syncANewDevice
            ? <SyncANewDeviceModal onClose={this.syncANewDevice} />
            : null
        }
        {
          this.state.resetSync
            ? <ResetSyncModal actions={actions} onClose={this.resetSync} />
            : null
        }
        <Card>
          <Grid columns='1fr 1fr'>
            <FlexColumn items='center'>
              <SwitchButton
                id='syncThisDevice'
                size='large'
                checked={syncData.setImmediateSyncDevice}
                onChange={this.toggleDeviceSyncing}
              />
              <SwitchLabel htmlFor='syncThisDevice'>{getLocale('syncThisDevice')}</SwitchLabel>
            </FlexColumn>
            <FlexColumn direction='column'>
              <Label>{getLocale('deviceName')}</Label>
              <Paragraph>{syncData.deviceName}</Paragraph>
            </FlexColumn>
          </Grid>
        </Card>
        <SectionBlock>
          <SubTitle level={2}>{getLocale('devices')}</SubTitle>
          <Table header={this.header} rows={this.rows} />
          <Button
            level='primary'
            type='accent'
            size='medium'
            text={getLocale('syncANewDevice')}
            onClick={this.syncANewDevice}
          />
        </SectionBlock>
        <SectionBlock>
          <SubTitle level={2}>{getLocale('syncData')}</SubTitle>
          <Paragraph>{getLocale('syncDataInfo')}</Paragraph>
          <Grid columns='auto 1fr' rows='1fr 1fr 1fr' gap='5px'>
            <SwitchButton
              id='bookmarks'
              checked={syncData.syncBookmarks}
              onChange={this.onChangeSyncBookmarks}
            />
            <SwitchLabel htmlFor='bookmarks'>
              {getLocale('bookmarks')}
            </SwitchLabel>
            <SwitchButton
              id='savedSiteSettings'
              checked={syncData.syncSavedSiteSettings}
              onChange={this.onChangeSavedSiteSettings}
            />
            <SwitchLabel htmlFor='savedSiteSettings'>
              {getLocale('savedSiteSettings')}
            </SwitchLabel>
            <SwitchButton
              id='browsingHistory'
              checked={syncData.syncBrowsingHistory}
              onChange={this.onChangeBrowsingHistory}
            />
            <SwitchLabel htmlFor='browsingHistory'>
              {getLocale('browsingHistory')}
            </SwitchLabel>
          </Grid>
        </SectionBlock>
        <SectionBlock>
          <SubTitle level={2}>{getLocale('clearData')}</SubTitle>
          <Button
            level='primary'
            type='accent'
            size='medium'
            text={getLocale('resetSync')}
            onClick={this.resetSync}
          />
        </SectionBlock>
      </>
    )
  }
}

export default SyncEnabledContent
