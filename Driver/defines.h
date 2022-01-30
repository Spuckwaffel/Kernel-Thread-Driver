#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <IntSafe.h>
#include <ntimage.h>

ULONG64 CODE_ADDRESS;
ULONG64 OUTPUT_ADDRESS;


PCWSTR SREGP = {};
PCWSTR SCODE = {};
PCWSTR SADDY = {};


#define print(fmt, ...) DbgPrintEx(0, 0, fmt, ##__VA_ARGS__)

#include "D:\_old\files\reset_9\repos\SpuckwareDriver1\SpuckwareDriver1\skCrypter.h"


#include "stdint.h"

#include "funcs.h"
#include "utilities.h"
#include "events.h"
