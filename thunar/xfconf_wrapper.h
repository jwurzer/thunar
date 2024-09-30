#ifndef XFCONF_WRAPPER_H
#define XFCONF_WRAPPER_H

#include <glib.h>
#include <glib-object.h>
#include <gobject/gtype.h>

G_BEGIN_DECLS

// from xfconf-channel.h
//#define XFCONF_TYPE_CHANNEL             (xfconf_channel_get_type())
//#define XFCONF_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), XFCONF_TYPE_CHANNEL, XfconfChannel))
//#define XFCONF_IS_CHANNEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), XFCONF_TYPE_CHANNEL))

typedef struct _XfconfChannel XfconfChannel;

// from xfconf.h

gboolean xfconf_init(GError **error);
// TODO: maybe thunar should call xfconf_shutdown()?
//void xfconf_shutdown(void);

// from xfconf-channel.h

XfconfChannel *xfconf_channel_get(const gchar *channel_name);
gboolean xfconf_channel_set_string(XfconfChannel *channel,
                                   const gchar *property,
                                   const gchar *value);
gchar **xfconf_channel_get_string_list(XfconfChannel *channel,
                                       const gchar *property) G_GNUC_WARN_UNUSED_RESULT;
gboolean xfconf_channel_has_property(XfconfChannel *channel,
                                     const gchar *property);
// called with recursive = FALSE
void xfconf_channel_reset_property(XfconfChannel *channel,
                                   const gchar *property_base,
                                   gboolean recursive);
gboolean xfconf_channel_get_property(XfconfChannel *channel,
                                     const gchar *property,
                                     GValue *value);
gboolean xfconf_channel_set_property(XfconfChannel *channel,
                                     const gchar *property,
                                     const GValue *value);
gboolean xfconf_channel_set_string_list(XfconfChannel *channel,
                                        const gchar *property,
                                        const gchar * const *values);

G_END_DECLS


#endif

