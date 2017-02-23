
#ifndef __test_MARSHAL_H__
#define __test_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:INT,BOOLEAN,CHAR,UCHAR,UINT,LONG,ULONG,ENUM,FLAGS,FLOAT,DOUBLE,STRING,PARAM,BOXED,POINTER,OBJECT,VARIANT,INT64,UINT64 (./marshalers.list:1) */
extern void test_VOID__INT_BOOLEAN_CHAR_UCHAR_UINT_LONG_ULONG_ENUM_FLAGS_FLOAT_DOUBLE_STRING_PARAM_BOXED_POINTER_OBJECT_VARIANT_INT64_UINT64 (GClosure     *closure,
                                                                                                                                              GValue       *return_value,
                                                                                                                                              guint         n_param_values,
                                                                                                                                              const GValue *param_values,
                                                                                                                                              gpointer      invocation_hint,
                                                                                                                                              gpointer      marshal_data);
extern void test_VOID__INT_BOOLEAN_CHAR_UCHAR_UINT_LONG_ULONG_ENUM_FLAGS_FLOAT_DOUBLE_STRING_PARAM_BOXED_POINTER_OBJECT_VARIANT_INT64_UINT64v (GClosure     *closure,
                                                                                                                                               GValue       *return_value,
                                                                                                                                               gpointer      instance,
                                                                                                                                               va_list       args,
                                                                                                                                               gpointer      marshal_data,
                                                                                                                                               int           n_params,
                                                                                                                                               GType        *param_types);

/* INT:VOID (./marshalers.list:2) */
extern void test_INT__VOID (GClosure     *closure,
                            GValue       *return_value,
                            guint         n_param_values,
                            const GValue *param_values,
                            gpointer      invocation_hint,
                            gpointer      marshal_data);
extern void test_INT__VOIDv (GClosure     *closure,
                             GValue       *return_value,
                             gpointer      instance,
                             va_list       args,
                             gpointer      marshal_data,
                             int           n_params,
                             GType        *param_types);

/* UINT:VOID (./marshalers.list:3) */
extern void test_UINT__VOID (GClosure     *closure,
                             GValue       *return_value,
                             guint         n_param_values,
                             const GValue *param_values,
                             gpointer      invocation_hint,
                             gpointer      marshal_data);
extern void test_UINT__VOIDv (GClosure     *closure,
                              GValue       *return_value,
                              gpointer      instance,
                              va_list       args,
                              gpointer      marshal_data,
                              int           n_params,
                              GType        *param_types);

G_END_DECLS

#endif /* __test_MARSHAL_H__ */

