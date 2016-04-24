#pragma once

#include <string>

void _stdcall AssociateObject(void* original_class, void* attached_class);
void* _stdcall GetAssociated(void* original_class);
void _stdcall FreeAssociated(void* original_class);