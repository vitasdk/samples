
#ifndef _PSP2_SAMPLE_LIB_KERNEL_H_
#define _PSP2_SAMPLE_LIB_KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

SceUInt64 sample_kern_get_module_start_time(void);

int sample_kern_get_module_start_time_for_user(SceUInt64 *time);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2_SAMPLE_LIB_KERNEL_H_ */
