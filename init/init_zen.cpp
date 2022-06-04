/*
 * Copyright (C) 2021 LineageOS Project
 *
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fstream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/properties.h>
#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using std::string;

std::vector<string> ro_product_props_default_source_order = {
    "",
    "odm",
    "vendor",
    "product",
    "system_ext",
    "system",
};

void property_override(char const prop[], char const value[], bool add = true) {
    prop_info *pi;

    pi = (prop_info *)__system_property_find(prop);

    if (pi)
	__system_property_update(pi, value, strlen(value));
    else if (add)
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_override_dual(char const prop[], char const system_prop[], char const value[])
{
    property_override(prop, value);
    property_override(system_prop, value);
}

void set_rild_libpath(char const variant[])
{
    string libpath("/vendor/lib64/libril");
    libpath += variant;
    libpath += ".so";

    property_override("rild.libpath", libpath.c_str());
}

void set_ro_product_prop(char const prop[], char const value[], char const other_value[])
{
    for (const auto &source : ro_product_props_default_source_order) {
		if ( other_value == "normal" ) {
			auto prop_name = "ro.product." + source + prop;
			property_override(prop_name.c_str(), value, false);
		} else if ( other_value == "qssi" ) {
			if ( source != "system_ext" ) {
				auto prop_name = "ro.product." + source + prop;
				property_override(prop_name.c_str(), value, false);
			} else if ( source == "system_ext" ) {
				auto prop_name = "ro.product." + source + prop;
				property_override(prop_name.c_str(), other_value, false);
			}
		}
    }
}

void set_ro_prop(char const prop[], char const value[])
{
    for (const auto &source : ro_product_props_default_source_order) {
        auto prop_name = "ro." + source + prop;
        property_override(prop_name.c_str(), value, false);
    }
}

void vendor_load_properties()
{
    string name;
    string country;
    string build_id;
    string build_number;
    std::ostringstream fp;
    string fingerprint;
    std::ostringstream desc;
    string description;
    std::ostringstream disp;
    string display_id;
    string region;
    string device;
    string model;

//   ASUS_I002D:/ $ getprop | grep ro.boot.country_code
//   [ro.boot.country_code]: [WW]
    region = name.substr (0,2);
    country = android::base::GetProperty("ro.boot.country_code", "");

    // These should be the only things to change for OTA updates
//  ro.build.id				RKQ1.200710.002
//  ro.odm.build.id			RKQ1.200710.002
//  ro.product.build.id		RKQ1.200710.002
//  ro.system.build.id		RKQ1.200710.002
//  ro.system_ext.build.id	RKQ1.200710.002
//  ro.vendor.build.id		RKQ1.200710.002
    build_id = android::base::GetProperty("ro.vendor.build.id", "");
    set_ro_prop("build.id", build_id.c_str());

//  ro.build.version.incremental			30.41.69.112_20210916
//  ro.odm.build.version.incremental		30.41.69.112_20210916
//  ro.product.build.version.incremental	30.41.69.112_20210916
//  ro.system.build.version.incremental		30.41.69.112_20210916
//  ro.system_ext.build.version.incremental	30.41.69.112_20210916
//  ro.vendor.build.version.incremental		30.41.69.112_20210916
    build_number = android::base::GetProperty("ro.vendor.build.version.incremental", "");
    set_ro_prop("build.version.incremental", build_number.c_str());

    //ASUS_I002D:/ $ getprop | grep description
    //[ro.build.description]: [kona-user 11 RKQ1.200710.002 30.41.69.112_20210916 release-keys]
    // Create the correct stock props based on the above values
    desc << "kona-user 11 " << build_id << " " << build_number << " release-keys";
    description = desc.str();

    //ASUS_I002D:/ $ getprop | grep ro.build.display.id
    //[ro.build.display.id]: [RKQ1.200710.002.30.41.69.112_20210916 release-keys]
    disp << build_id << "." << build_number << " release-keys";
    display_id = disp.str();

    //     asus/   WW_I002D   /ASUS_I002D:11/   RKQ1.200710.002/30.41.69.112_20210916 :user/release-keys
    fp << "asus/" << name << "/ASUS_I002D:11/" << build_id << "/" << build_number << ":user/release-keys";
    fingerprint = fp.str();

    // These properties are the same for all variants
    property_override("ro.build.description", description.c_str());
    property_override("ro.build.display.id", display_id.c_str());

//ASUS_I002D:/ $ getprop | grep carrier
//[persist.vendor.fota.group_carrier]: [ASUS-ASUS_I002D-WW]
//[ro.carrier]: [unknown]
//[ro.vendor.product.carrier]: [ASUS-ASUS_I002D-WW]
    property_override("ro.vendor.product.carrier", "ASUS-ASUS_I002D-WW");

//----------------------------------------------------------------------
//  ro.product.device				ASUS_I002D
//  ro.product.odm.device			ASUS_I002D
//  ro.product.product.device		ASUS_I002D
//  ro.product.system.device		ASUS_I002D
//  ro.product.system_ext.device	qssi
//  ro.product.vendor.device		ASUS_I002D
//
//   ASUS_I002D:/ $ getprop | grep ro.product.vendor.device
//   [ro.product.vendor.device]: [ASUS_I002D]
    device = android::base::GetProperty("ro.product.vendor.device", "");
    set_ro_product_prop("device", device.c_str(), "qssi");
//----------------------------------------------------------------------
//  ro.bootimage.build.fingerprint	asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
//:::::::::::::
//  ro.build.fingerprint			asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
//  ro.odm.build.fingerprint		asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
//  ro.product.build.fingerprint	asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
//  ro.system.build.fingerprint		asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
//  ro.system_ext.build.fingerprint	asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
//  ro.vendor.build.fingerprint		asus/WW_I002D/ASUS_I002D:11/RKQ1.200710.002/30.41.69.112_20210916:user/release-keys
    property_override("ro.bootimage.build.fingerprint", fingerprint.c_str());
    set_ro_prop("build.fingerprint", fingerprint.c_str());
//----------------------------------------------------------------------
//  ro.product.model				ASUS_I002D
//  ro.product.odm.model			ASUS_I002D
//  ro.product.product.model		ASUS_I002D
//  ro.product.system.model			ASUS_I002D
//  ro.product.system_ext.model		ASUS_I002D
//  ro.product.vendor.model			ASUS_I002D
//
//   ASUS_I002D:/ $ getprop | grep ro.product.vendor.model
//   [ro.product.vendor.model]: [ASUS_I002D]
    model = android::base::GetProperty("ro.product.vendor.model", "");
    set_ro_product_prop("model", model.c_str(), "normal");
//----------------------------------------------------------------------
//  ro.product.name					WW_I002D
//  ro.product.odm.name				WW_I002D
//  ro.product.product.name			WW_I002D
//  ro.product.system.name			WW_I002D
//  ro.product.system_ext.name		qssi
//  ro.product.vendor.name			WW_I002D
//
//   ASUS_I002D:/ $ getprop | grep ro.product.vendor.name
//   [ro.product.vendor.name]: [WW_I002D]
    name = android::base::GetProperty("ro.product.vendor.name", "");
    set_ro_product_prop("name", name.c_str(), "qssi");
//----------------------------------------------------------------------

    // Set below properties based on variant name
    if (name == "WW_I002D") {
        // Set properties for IN and US variants
        if (country == "IN") {
            property_override_dual("ro.product.model", "ro.product.system.model", "ASUS_I002DE");
            property_override("ro.config.versatility", "IN");
        } else if (country == "US") {
            property_override("ro.config.versatility", "US");
        } else if (country == "WW") {
//----------------------------------------------------------------------
			property_override("ro.telephony.default_network", "26,26");
			//  There is below code in vendor/etc/init/hw/init.qcom.rc
			//
			//  # Set vendor-ril lib path based on Meta version
            //  on property:vendor.rild.libpath=*
            //  setprop rild.libpath ${vendor.rild.libpath}
            //////////////////
            // [rild.libpath]: [/vendor/lib64/libril-qc-hal-qmi.so]
    		property_override("vendor.rild.libpath", "/vendor/lib64/libril-qc-hal-qmi.so");
			property_override("telephony.lteOnCdmaDevice", "1");
			property_override("ril.subscription.types", "RUIM,RUIM");
//----------------------------------------------------------------------
        } else {
            property_override_dual("ro.product.model", "ro.product.system.model", "ASUS_I002D");
        }
    } else {
        property_override("ro.product.system.name", name.c_str());
        property_override("ro.config.versatility", region.c_str());
    }
    if (name == "RU_I002D") {
        property_override_dual("ro.product.model", "ro.product.system.model", "ASUS_I002DB");
    }
    if (name == "EU_I002D") {
        property_override_dual("ro.product.model", "ro.product.system.model", "ASUS_I002DC");
    }
    if (name == "CN_I002D") {
        property_override_dual("ro.product.model", "ro.product.system.model", "ASUS_I002DB");
    }
}
