
#include "xfconf_wrapper.h"

// dummy implementations

gboolean xfconf_init(GError **error)
{
	g_warning("TODO: xfconf_init()");
	return FALSE;
}

// TODO: maybe thunar should call xfconf_shutdown()?
#if 0 // currently not necessary to implement
void xfconf_shutdown(void)
{
	g_warning("TODO: xfconf_shutdown()");
}
#endif

XfconfChannel *xfconf_channel_get(const gchar *channel_name)
{
	g_warning("TODO: xfconf_channel_get(const gchar *channel_name), channel_name: \"%s\"", channel_name);
	return NULL;
}

gboolean xfconf_channel_set_string(XfconfChannel *channel,
                                   const gchar *property,
                                   const gchar *value)
{
	g_warning("TODO: xfconf_channel_set_string()");
	return FALSE;
}

gchar **xfconf_channel_get_string_list(XfconfChannel *channel,
                                       const gchar *property)
{
	g_warning("TODO: xfconf_channel_get_string_list()");
	return NULL;
}

gboolean xfconf_channel_has_property(XfconfChannel *channel,
                                     const gchar *property)
{
	g_warning("TODO: xfconf_channel_has_property()");
	return FALSE;
}

void xfconf_channel_reset_property(XfconfChannel *channel,
                                   const gchar *property_base,
                                   gboolean recursive)
{
	g_warning("TODO: xfconf_channel_reset_property()");
}

gboolean xfconf_channel_get_property(XfconfChannel *channel,
                                     const gchar *property,
                                     GValue *value)
{
	g_warning("TODO: xfconf_channel_get_property()");
	return FALSE;
}

gboolean xfconf_channel_set_property(XfconfChannel *channel,
                                     const gchar *property,
                                     const GValue *value)
{
	g_warning("TODO: xfconf_channel_set_property()");
	return FALSE;
}

gboolean xfconf_channel_set_string_list(XfconfChannel *channel,
                                        const gchar *property,
                                        const gchar * const *values)
{
	g_warning("TODO: xfconf_channel_set_string_list()");
	return FALSE;
}

