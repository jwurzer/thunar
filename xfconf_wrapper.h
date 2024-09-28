#ifndef XFCONF_WRAPPER_H
#define XFCONF_WRAPPER_H

#include <glib.h>
#include <glib-object.h>
#include <gobject/gtype.h>

G_BEGIN_DECLS

// from xfconf-channel.h
#define XFCONF_TYPE_CHANNEL             (xfconf_channel_get_type())
#define XFCONF_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), XFCONF_TYPE_CHANNEL, XfconfChannel))
#define XFCONF_IS_CHANNEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), XFCONF_TYPE_CHANNEL))

typedef struct _XfconfChannel XfconfChannel;
static GType xfconf_channel_get_type(void) G_GNUC_CONST;
static XfconfChannel *xfconf_channel_get(const gchar *channel_name);
static XfconfChannel *xfconf_channel_new(const gchar *channel_name) G_GNUC_WARN_UNUSED_RESULT;
static gboolean xfconf_channel_get_bool(XfconfChannel *channel,
                                 const gchar *property,
                                 gboolean default_value);
static gchar **xfconf_channel_get_string_list(XfconfChannel *channel,
                                       const gchar *property) G_GNUC_WARN_UNUSED_RESULT;
static gboolean xfconf_channel_set_string_list(XfconfChannel *channel,
                                        const gchar *property,
                                        const gchar * const *values);
static gchar *xfconf_channel_get_string(XfconfChannel *channel,
                                 const gchar *property,
                                 const gchar *default_value) G_GNUC_WARN_UNUSED_RESULT;
static gboolean xfconf_channel_set_string(XfconfChannel *channel,
                                   const gchar *property,
                                   const gchar *value);
static gboolean xfconf_channel_has_property(XfconfChannel *channel,
                                     const gchar *property);
static void xfconf_channel_reset_property(XfconfChannel *channel,
                                   const gchar *property_base,
                                   gboolean recursive);
static GHashTable *xfconf_channel_get_properties(XfconfChannel *channel,
                                          const gchar *property_base) G_GNUC_WARN_UNUSED_RESULT;
static gboolean xfconf_channel_set_property(XfconfChannel *channel,
                                     const gchar *property,
                                     const GValue *value);
static gboolean xfconf_channel_set_bool(XfconfChannel *channel,
                                 const gchar *property,
                                 gboolean value);
static gboolean xfconf_channel_set_array(XfconfChannel *channel,
                                  const gchar *property,
                                  GType first_value_type,
                                  ...);
static gint32 xfconf_channel_get_int(XfconfChannel *channel,
                              const gchar *property,
                              gint32 default_value);
static gboolean xfconf_channel_is_property_locked(XfconfChannel *channel,
                                           const gchar *property);
static gboolean xfconf_channel_get_property(XfconfChannel *channel,
                                     const gchar *property,
                                     GValue *value);
static GPtrArray *xfconf_channel_get_arrayv(XfconfChannel *channel,
                                     const gchar *property) G_GNUC_WARN_UNUSED_RESULT;
static gboolean xfconf_channel_set_arrayv(XfconfChannel *channel,
                                   const gchar *property,
                                   GPtrArray *values);
static gboolean xfconf_channel_set_uint(XfconfChannel *channel,
                                 const gchar *property,
                                 guint32 value);
static gboolean xfconf_channel_set_int(XfconfChannel *channel,
                                const gchar *property,
                                gint32 value);
static guint32 xfconf_channel_get_uint(XfconfChannel *channel,
                                const gchar *property,
                                guint32 default_value);

// from xfconf-binding.h
static gulong xfconf_g_property_bind(XfconfChannel *channel,
                              const gchar *xfconf_property,
                              GType xfconf_property_type,
                              gpointer object,
                              const gchar *object_property);
static gulong xfconf_g_property_bind_gdkrgba(XfconfChannel *channel,
                                      const gchar *xfconf_property,
                                      gpointer object,
                                      const gchar *object_property);
static void xfconf_g_property_unbind_all(gpointer channel_or_object);

// from xfconf.h

static gboolean xfconf_init(GError **error);
static void xfconf_shutdown(void);
static void xfconf_array_free(GPtrArray *arr);

G_END_DECLS

// dummy implementations

static gboolean xfconf_init(GError **error)
{
	g_warning("TODO: xfconf_init()");
	return FALSE;
}

static void xfconf_shutdown(void)
{
	g_warning("TODO: xfconf_shutdown()");
}

static void xfconf_array_free(GPtrArray *arr)
{
	g_warning("TODO: xfconf_array_free(GPtrArray *arr)");
}

static GType xfconf_channel_get_type(void)
{
	g_warning("TODO: xfconf_channel_get_type(void)");
	return 0; // TODO
}

static XfconfChannel *xfconf_channel_get(const gchar *channel_name)
{
	g_warning("TODO: xfconf_channel_get(const gchar *channel_name), channel_name: \"%s\"", channel_name);
	return NULL;
}

static XfconfChannel *xfconf_channel_new(const gchar *channel_name)
{
	g_warning("TODO: xfconf_channel_new(const gchar *channel_name), channel_name: \"%s\"", channel_name);
	return NULL;
}

static gboolean xfconf_channel_get_bool(XfconfChannel *channel,
                                 const gchar *property,
                                 gboolean default_value)
{
	g_warning("TODO: xfconf_channel_get_bool()");
	return FALSE;
}

static gchar **xfconf_channel_get_string_list(XfconfChannel *channel,
                                       const gchar *property)
{
	g_warning("TODO: xfconf_channel_get_string_list()");
	return NULL;
}

static gboolean xfconf_channel_set_string_list(XfconfChannel *channel,
                                        const gchar *property,
                                        const gchar * const *values)
{
	g_warning("TODO: xfconf_channel_set_string_list()");
	return FALSE;
}

static gchar *xfconf_channel_get_string(XfconfChannel *channel,
                                 const gchar *property,
                                 const gchar *default_value)
{
	g_warning("TODO: xfconf_channel_get_string()");
	return NULL;
}

static gboolean xfconf_channel_set_string(XfconfChannel *channel,
                                   const gchar *property,
                                   const gchar *value)
{
	g_warning("TODO: xfconf_channel_set_string()");
	return FALSE;
}

static gboolean xfconf_channel_has_property(XfconfChannel *channel,
                                     const gchar *property)
{
	g_warning("TODO: xfconf_channel_has_property()");
	return FALSE;
}

static void xfconf_channel_reset_property(XfconfChannel *channel,
                                   const gchar *property_base,
                                   gboolean recursive)
{
	g_warning("TODO: xfconf_channel_reset_property()");
}

static GHashTable *xfconf_channel_get_properties(XfconfChannel *channel,
                                          const gchar *property_base)
{
	g_warning("TODO: xfconf_channel_get_properties()");
	return NULL;
}

static gboolean xfconf_channel_set_property(XfconfChannel *channel,
                                     const gchar *property,
                                     const GValue *value)
{
	g_warning("TODO: xfconf_channel_set_property()");
	return FALSE;
}

static gboolean xfconf_channel_set_bool(XfconfChannel *channel,
                                 const gchar *property,
                                 gboolean value)
{
	g_warning("TODO: xfconf_channel_set_bool()");
	return FALSE;
}

static gboolean xfconf_channel_set_array(XfconfChannel *channel,
                                  const gchar *property,
                                  GType first_value_type,
                                  ...)
{
	g_warning("TODO: xfconf_channel_set_array()");
	return FALSE;
}

static gint32 xfconf_channel_get_int(XfconfChannel *channel,
                              const gchar *property,
                              gint32 default_value)
{
	g_warning("TODO: xfconf_channel_get_int()");
	return 0;
}

static gboolean xfconf_channel_is_property_locked(XfconfChannel *channel,
                                           const gchar *property)
{
	g_warning("TODO: xfconf_channel_is_property_locked()");
	return FALSE;
}

static gboolean xfconf_channel_get_property(XfconfChannel *channel,
                                     const gchar *property,
                                     GValue *value)
{
	g_warning("TODO: xfconf_channel_get_property()");
	return FALSE;
}

static GPtrArray *xfconf_channel_get_arrayv(XfconfChannel *channel,
                                     const gchar *property)
{
	g_warning("TODO: xfconf_channel_get_arrayv()");
	return NULL;
}

static gboolean xfconf_channel_set_arrayv(XfconfChannel *channel,
                                   const gchar *property,
                                   GPtrArray *values)
{
	g_warning("TODO: xfconf_channel_set_arrayv()");
	return FALSE;
}

static gboolean xfconf_channel_set_uint(XfconfChannel *channel,
                                 const gchar *property,
                                 guint32 value)
{
	g_warning("TODO: xfconf_channel_set_uint()");
	return FALSE;
}

static gboolean xfconf_channel_set_int(XfconfChannel *channel,
                                const gchar *property,
                                gint32 value)
{
	g_warning("TODO: xfconf_channel_set_int()");
	return FALSE;
}

static guint32 xfconf_channel_get_uint(XfconfChannel *channel,
                                const gchar *property,
                                guint32 default_value)
{
	g_warning("TODO: xfconf_channel_get_uint()");
	return 0;
}

static gulong xfconf_g_property_bind(XfconfChannel *channel,
                              const gchar *xfconf_property,
                              GType xfconf_property_type,
                              gpointer object,
                              const gchar *object_property)
{
	g_warning("TODO: xfconf_g_property_bind(...)");
	return 0;
}

static gulong xfconf_g_property_bind_gdkrgba(XfconfChannel *channel,
                                      const gchar *xfconf_property,
                                      gpointer object,
                                      const gchar *object_property)
{
	g_warning("TODO: xfconf_g_property_bind_gdkrgba()");
	return 0;
}

static void xfconf_g_property_unbind_all(gpointer channel_or_object)
{
	g_warning("TODO: xfconf_g_property_unbind_all()");
}

#endif

