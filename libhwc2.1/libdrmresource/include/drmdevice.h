/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_DRM_H_
#define ANDROID_DRM_H_

#include "drmconnector.h"
#include "drmcrtc.h"
#include "drmencoder.h"
#include "drmeventlistener.h"
#include "drmplane.h"

#include <map>
#include <stdint.h>
#include <tuple>

namespace android {

class DrmDevice {
 public:
  DrmDevice();
  ~DrmDevice();

  std::tuple<int, int> Init(const char *path, int num_displays);

  int fd() const {
    return fd_.get();
  }

  const std::vector<std::unique_ptr<DrmConnector>> &connectors() const {
    return connectors_;
  }

  const std::vector<std::unique_ptr<DrmPlane>> &planes() const {
    return planes_;
  }

  std::pair<uint32_t, uint32_t> min_resolution() const {
    return min_resolution_;
  }

  std::pair<uint32_t, uint32_t> max_resolution() const {
    return max_resolution_;
  }

  DrmConnector *GetConnectorForDisplay(int display) const;
  DrmConnector *GetWritebackConnectorForDisplay(int display) const;
  DrmConnector *AvailableWritebackConnector(int display) const;
  DrmCrtc *GetCrtcForDisplay(int display) const;
  DrmPlane *GetPlane(uint32_t id) const;
  DrmEventListener *event_listener();

  int GetPlaneProperty(const DrmPlane &plane, const char *prop_name,
                       DrmProperty *property);
  int GetCrtcProperty(const DrmCrtc &crtc, const char *prop_name,
                      DrmProperty *property);
  int GetConnectorProperty(const DrmConnector &connector, const char *prop_name,
                           DrmProperty *property);
  int UpdateCrtcProperty(const DrmCrtc &crtc, DrmProperty *property);
  int UpdateConnectorProperty(const DrmConnector &conn, DrmProperty *property);

  const std::vector<std::unique_ptr<DrmCrtc>> &crtcs() const;
  uint32_t next_mode_id();

  int CreatePropertyBlob(const void *data, size_t length, uint32_t *blob_id);
  int DestroyPropertyBlob(uint32_t blob_id);
  bool HandlesDisplay(int display) const;
  void RegisterHotplugHandler(const std::shared_ptr<DrmEventHandler> &handler) {
    event_listener_.RegisterHotplugHandler(handler);
  }
  void RegisterHistogramHandler(const std::shared_ptr<DrmHistogramEventHandler> &handler) {
    event_listener_.RegisterHistogramHandler(handler);
  }
  void RegisterHistogramChannelHandler(
      const std::shared_ptr<DrmHistogramChannelEventHandler> &handler) {
    event_listener_.RegisterHistogramChannelHandler(handler);
  }
  void RegisterContextHistogramHandler(
      const std::shared_ptr<DrmContextHistogramEventHandler> &handler) {
    event_listener_.RegisterContextHistogramHandler(handler);
  }

  int CallVendorIoctl(unsigned long request, void *arg);

  private:
  int UpdateObjectProperty(int id, int type, DrmProperty *property);
  int TryEncoderForDisplay(int display, DrmEncoder *enc);
  int GetProperty(uint32_t obj_id, uint32_t obj_type, const char *prop_name,
                  DrmProperty *property);

  int CreateDisplayPipe(DrmConnector *connector);
  int AttachWriteback(DrmConnector *display_conn);

  UniqueFd fd_;
  uint32_t mode_id_ = 0;

  std::vector<std::unique_ptr<DrmConnector>> connectors_;
  std::vector<std::unique_ptr<DrmConnector>> writeback_connectors_;
  std::vector<std::unique_ptr<DrmEncoder>> encoders_;
  std::vector<std::unique_ptr<DrmCrtc>> crtcs_;
  std::vector<std::unique_ptr<DrmPlane>> planes_;
  DrmEventListener event_listener_;

  std::pair<uint32_t, uint32_t> min_resolution_;
  std::pair<uint32_t, uint32_t> max_resolution_;
  std::map<int, int> displays_;
};
}  // namespace android

#endif  // ANDROID_DRM_H_
