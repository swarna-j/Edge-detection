// ==============================================================
// File generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2016.4
// Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
// 
// ==============================================================

#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xedge_detect.h"

extern XEdge_detect_Config XEdge_detect_ConfigTable[];

XEdge_detect_Config *XEdge_detect_LookupConfig(u16 DeviceId) {
	XEdge_detect_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XEDGE_DETECT_NUM_INSTANCES; Index++) {
		if (XEdge_detect_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XEdge_detect_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XEdge_detect_Initialize(XEdge_detect *InstancePtr, u16 DeviceId) {
	XEdge_detect_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XEdge_detect_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XEdge_detect_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

