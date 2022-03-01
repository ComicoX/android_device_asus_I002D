# Copyright (C) 2020-2022 ComicoX
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file is the build configuration for a full Android
# build for grouper hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#

$(call inherit-product, device/asus/I002D/device.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)

TARGET_BOOTANIMATION_SIZE := 1080p

# Inherit HavocOS.
$(call inherit-product, vendor/havoc/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_BRAND := asus
PRODUCT_DEVICE := I002D
PRODUCT_MANUFACTURER := asus
PRODUCT_MODEL := ASUS_I002D
PRODUCT_NAME := havoc_I002D
PRODUCT_SYSTEM_DEVICE := ASUS_I002
PRODUCT_SYSTEM_NAME := WW_I002D

PRODUCT_GMS_CLIENTID_BASE := android-asus

TARGET_DEVICE := WW_I002D

# Official
HAVOC_BUILD_TYPE := Official

PRODUCT_SYSTEM_DEVICE := ASUS_I002D
PRODUCT_SYSTEM_NAME := WW_I002D

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_DEVICE=ASUS_I002D \
    PRODUCT_NAME=WW_I002D \
    TARGET_DEVICE=ZS670KS

VENDOR_RELEASE := 11/RKQ1.200710.002/30.41.69.152_20220128:user/release-keys
BUILD_FINGERPRINT := asus/WW_I002D/ASUS_I002D:$(VENDOR_RELEASE)

PLATFORM_SECURITY_PATCH_OVERRIDE := 2021-12-05
