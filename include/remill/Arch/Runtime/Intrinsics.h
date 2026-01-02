/*
 * Copyright (c) 2017 Trail of Bits, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "remill/Arch/Runtime/Definitions.h"
#include "remill/Arch/Runtime/HyperCall.h"
#include "remill/Arch/Runtime/Types.h"

extern "C" {

// Memory read intrinsics.
USED extern uint8_t __remill_read_memory_8(Memory *, addr_t);

USED extern uint16_t __remill_read_memory_16(Memory *, addr_t);

USED extern uint32_t __remill_read_memory_32(Memory *, addr_t);

USED extern uint64_t __remill_read_memory_64(Memory *, addr_t);

// Memory write intrinsics.
USED extern Memory *__remill_write_memory_8(Memory *, addr_t, uint8_t);

USED extern Memory *__remill_write_memory_16(Memory *, addr_t, uint16_t);

USED extern Memory *__remill_write_memory_32(Memory *, addr_t, uint32_t);

USED extern Memory *__remill_write_memory_64(Memory *, addr_t, uint64_t);

USED extern float32_t __remill_read_memory_f32(Memory *, addr_t);

USED extern float64_t __remill_read_memory_f64(Memory *, addr_t);

USED extern Memory *__remill_read_memory_f80(Memory *, addr_t, float80_t &);

USED extern float128_t __remill_read_memory_f128(Memory *, addr_t);

USED extern Memory *__remill_write_memory_f32(Memory *, addr_t, float32_t);

USED extern Memory *__remill_write_memory_f64(Memory *, addr_t, float64_t);

USED extern Memory *__remill_write_memory_f80(Memory *, addr_t,
                                              const float80_t &);

USED extern Memory *__remill_write_memory_f128(Memory *, addr_t, float128_t);

USED extern uint8_t __remill_undefined_8(void);

USED extern uint16_t __remill_undefined_16(void);

USED extern uint32_t __remill_undefined_32(void);

USED extern uint64_t __remill_undefined_64(void);

USED extern float32_t __remill_undefined_f32(void);

USED extern float64_t __remill_undefined_f64(void);

USED extern float80_t __remill_undefined_f80(void);

USED extern float128_t __remill_undefined_f128(void);

USED extern bool __remill_flag_computation_zero(bool result, ...);

USED extern bool __remill_flag_computation_sign(bool result, ...);

USED extern bool __remill_flag_computation_overflow(bool result, ...);

USED extern bool __remill_flag_computation_carry(bool result, ...);

USED extern bool __remill_compare_sle(bool result);

USED extern bool __remill_compare_slt(bool result);

USED extern bool __remill_compare_sge(bool result);

USED extern bool __remill_compare_sgt(bool result);


USED extern bool __remill_compare_ule(bool result);

USED extern bool __remill_compare_ult(bool result);

USED extern bool __remill_compare_ugt(bool result);

USED extern bool __remill_compare_uge(bool result);

USED extern bool __remill_compare_eq(bool result);

USED extern bool __remill_compare_neq(bool result);

// Generic error.
USED extern Memory *__remill_error(State &, addr_t addr, Memory *);

// Control-flow intrinsics.
USED extern Memory *__remill_function_call(State &, addr_t addr, Memory *);

USED extern Memory *__remill_function_return(State &, addr_t addr, Memory *);

USED extern Memory *__remill_jump(State &, addr_t addr, Memory *);

USED extern Memory *__remill_missing_block(State &, addr_t addr, Memory *);

USED extern Memory *__remill_async_hyper_call(State &, addr_t ret_addr,
                                              Memory *);

// This intrinsic must be tagged with the `always_inline` function attribute
// since it has an implementation we want to use in Anvill's lifted IR.
//
// Without this attribute, the function will be be marked with `noinline` in
// each architecture's runtime. During optimization, the intrinsic calls will
// not be inlined. Then, when the lifted functions are moved to the target
// module, the intrinsic implementation won't be moved across and will be lost,
// leaving calls to it in the lifted IR.
USED ALWAYS_INLINE extern Memory *__remill_sync_hyper_call(State &, Memory *,
                                                           SyncHyperCall::Name);

// Memory barriers types:
//  http://g.oswego.edu/dl/jmm/cookbook.html
//  http://preshing.com/20120913/acquire-and-release-semantics/
//  http://preshing.com/20120710/memory-barriers-are-like-source-control-operations/
USED extern Memory *__remill_barrier_load_load(Memory *);

USED extern Memory *__remill_barrier_load_store(Memory *);  // Load acquire.

USED extern Memory *__remill_barrier_store_load(Memory *);

USED extern Memory *__remill_barrier_store_store(Memory *);  // Store release.

// Atomic operations. The address/size are hints, but the granularity of the
// access can be bigger. These have implicit StoreLoad semantics.
USED extern Memory *__remill_atomic_begin(Memory *);

USED extern Memory *__remill_atomic_end(Memory *);

// Used to signal the begin/ending of an instruction executed within a delay
// slot.
USED extern Memory *__remill_delay_slot_begin(Memory *);

USED extern Memory *__remill_delay_slot_end(Memory *);

USED extern Memory *__remill_compare_exchange_memory_8(Memory *, addr_t addr,
                                                       uint8_t &expected,
                                                       uint8_t desired);

USED extern Memory *__remill_compare_exchange_memory_16(Memory *, addr_t addr,
                                                        uint16_t &expected,
                                                        uint16_t desired);

USED extern Memory *__remill_compare_exchange_memory_32(Memory *, addr_t addr,
                                                        uint32_t &expected,
                                                        uint32_t desired);

USED extern Memory *__remill_compare_exchange_memory_64(Memory *, addr_t addr,
                                                        uint64_t &expected,
                                                        uint64_t desired);

#if !defined(REMILL_DISABLE_INT128)
USED extern Memory *__remill_compare_exchange_memory_128(Memory *, addr_t addr,
                                                         uint128_t &expected,
                                                         uint128_t &desired);
#endif

USED extern Memory *__remill_fetch_and_add_8(Memory *, addr_t addr,
                                             uint8_t &value);

USED extern Memory *__remill_fetch_and_add_16(Memory *, addr_t addr,
                                              uint16_t &value);

USED extern Memory *__remill_fetch_and_add_32(Memory *, addr_t addr,
                                              uint32_t &value);

USED extern Memory *__remill_fetch_and_add_64(Memory *, addr_t addr,
                                              uint64_t &value);

USED extern Memory *__remill_fetch_and_sub_8(Memory *, addr_t addr,
                                             uint8_t &value);

USED extern Memory *__remill_fetch_and_sub_16(Memory *, addr_t addr,
                                              uint16_t &value);

USED extern Memory *__remill_fetch_and_sub_32(Memory *, addr_t addr,
                                              uint32_t &value);

USED extern Memory *__remill_fetch_and_sub_64(Memory *, addr_t addr,
                                              uint64_t &value);

USED extern Memory *__remill_fetch_and_and_8(Memory *, addr_t addr,
                                             uint8_t &value);

USED extern Memory *__remill_fetch_and_and_16(Memory *, addr_t addr,
                                              uint16_t &value);

USED extern Memory *__remill_fetch_and_and_32(Memory *, addr_t addr,
                                              uint32_t &value);

USED extern Memory *__remill_fetch_and_and_64(Memory *, addr_t addr,
                                              uint64_t &value);

USED extern Memory *__remill_fetch_and_or_8(Memory *, addr_t addr,
                                            uint8_t &value);

USED extern Memory *__remill_fetch_and_or_16(Memory *, addr_t addr,
                                             uint16_t &value);

USED extern Memory *__remill_fetch_and_or_32(Memory *, addr_t addr,
                                             uint32_t &value);

USED extern Memory *__remill_fetch_and_or_64(Memory *, addr_t addr,
                                             uint64_t &value);

USED extern Memory *__remill_fetch_and_xor_8(Memory *, addr_t addr,
                                             uint8_t &value);

USED extern Memory *__remill_fetch_and_xor_16(Memory *, addr_t addr,
                                              uint16_t &value);

USED extern Memory *__remill_fetch_and_xor_32(Memory *, addr_t addr,
                                              uint32_t &value);

USED extern Memory *__remill_fetch_and_xor_64(Memory *, addr_t addr,
                                              uint64_t &value);

USED extern Memory *__remill_fetch_and_nand_8(Memory *, addr_t addr,
                                              uint8_t &value);

USED extern Memory *__remill_fetch_and_nand_16(Memory *, addr_t addr,
                                               uint16_t &value);

USED extern Memory *__remill_fetch_and_nand_32(Memory *, addr_t addr,
                                               uint32_t &value);

USED extern Memory *__remill_fetch_and_nand_64(Memory *, addr_t addr,
                                               uint64_t &value);

// Read current floating point exception flags.
// Uses architecture-specific FPUExceptionFlag values that are mapped to
// cfenv flags. Typically implemented via std::fetestexcept.
// NOTE: You need to use BarrierReorder around this to avoid reordering bugs.
USED extern int32_t __remill_fpu_exception_test(int32_t read_mask);

// Clear floating point exception flags.
// Uses architecture-specific FPUExceptionFlag values that are mapped to
// cfenv flags. Typically implemented via std::feclearexcept.
// NOTE: You need to use BarrierReorder around this to avoid reordering bugs.
USED extern void __remill_fpu_exception_clear(int32_t clear_mask);

// Raise floating point exception flags.
// Uses architecture-specific FPUExceptionFlag values that are mapped to
// cfenv flags. Typically implemented via std::feraiseexcept.
// NOTE: You need to use BarrierReorder around this to avoid reordering bugs.
USED extern void __remill_fpu_exception_raise(int32_t except_mask);

// Set the floating point rounding mode.
// Uses architecture-specific FPURoundingControl values that are mapped to
// cfenv rounding modes. Typically implemented via std::fesetround.
// NOTE: You need to use BarrierReorder around this to avoid reordering bugs.
USED extern void __remill_fpu_set_rounding(int32_t round_mode);

// Get the current floating point rounding mode.
// Returns architecture-specific FPURoundingControl values mapped from
// cfenv rounding modes. Typically implemented via std::fegetround.
// NOTE: You need to use BarrierReorder around this to avoid reordering bugs.
USED extern int32_t __remill_fpu_get_rounding();

// Read/write to I/O ports.
USED extern uint8_t __remill_read_io_port_8(Memory *, addr_t);

USED extern uint16_t __remill_read_io_port_16(Memory *, addr_t);

USED extern uint32_t __remill_read_io_port_32(Memory *, addr_t);

USED extern Memory *__remill_write_io_port_8(Memory *, addr_t, uint8_t);

USED extern Memory *__remill_write_io_port_16(Memory *, addr_t, uint16_t);

USED extern Memory *__remill_write_io_port_32(Memory *, addr_t, uint32_t);

// More specific hyper calls.
USED extern Memory *__remill_x86_set_segment_es(Memory *);

USED extern Memory *__remill_x86_set_segment_ss(Memory *);

USED extern Memory *__remill_x86_set_segment_ds(Memory *);

USED extern Memory *__remill_x86_set_segment_fs(Memory *);

USED extern Memory *__remill_x86_set_segment_gs(Memory *);

USED extern Memory *__remill_x86_set_debug_reg(Memory *);

USED extern Memory *__remill_x86_set_control_reg_0(Memory *);

USED extern Memory *__remill_x86_set_control_reg_1(Memory *);

USED extern Memory *__remill_x86_set_control_reg_2(Memory *);

USED extern Memory *__remill_x86_set_control_reg_3(Memory *);

USED extern Memory *__remill_x86_set_control_reg_4(Memory *);

USED extern Memory *__remill_amd64_set_debug_reg(Memory *);

USED extern Memory *__remill_amd64_set_control_reg_0(Memory *);

USED extern Memory *__remill_amd64_set_control_reg_1(Memory *);

USED extern Memory *__remill_amd64_set_control_reg_2(Memory *);

USED extern Memory *__remill_amd64_set_control_reg_3(Memory *);

USED extern Memory *__remill_amd64_set_control_reg_4(Memory *);

USED extern Memory *__remill_amd64_set_control_reg_8(Memory *);

USED extern Memory *__remill_aarch64_emulate_instruction(Memory *);

USED extern Memory *__remill_aarch32_emulate_instruction(Memory *);

USED extern Memory *__remill_aarch32_check_not_el2(Memory *);

USED extern Memory *__remill_sparc_set_asi_register(Memory *);

USED extern Memory *__remill_sparc_unimplemented_instruction(Memory *);

USED extern Memory *__remill_sparc_unhandled_dcti(Memory *);

USED extern Memory *__remill_sparc_window_underflow(Memory *);

USED extern Memory *__remill_sparc_trap_cond_a(Memory *);

USED extern Memory *__remill_sparc_trap_cond_n(Memory *);

USED extern Memory *__remill_sparc_trap_cond_ne(Memory *);

USED extern Memory *__remill_sparc_trap_cond_e(Memory *);

USED extern Memory *__remill_sparc_trap_cond_g(Memory *);

USED extern Memory *__remill_sparc_trap_cond_le(Memory *);

USED extern Memory *__remill_sparc_trap_cond_ge(Memory *);

USED extern Memory *__remill_sparc_trap_cond_l(Memory *);

USED extern Memory *__remill_sparc_trap_cond_gu(Memory *);

USED extern Memory *__remill_sparc_trap_cond_leu(Memory *);

USED extern Memory *__remill_sparc_trap_cond_cc(Memory *);

USED extern Memory *__remill_sparc_trap_cond_cs(Memory *);

USED extern Memory *__remill_sparc_trap_cond_pos(Memory *);

USED extern Memory *__remill_sparc_trap_cond_neg(Memory *);

USED extern Memory *__remill_sparc_trap_cond_vc(Memory *);

USED extern Memory *__remill_sparc_trap_cond_vs(Memory *);

USED extern Memory *__remill_sparc32_emulate_instruction(Memory *);

USED extern Memory *__remill_sparc64_emulate_instruction(Memory *);

USED extern Memory *__remill_ppc_emulate_instruction(Memory *);

USED extern Memory *__remill_ppc_syscall(Memory *);

}  // extern C
