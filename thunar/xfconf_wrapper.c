
#include "xfconf_wrapper.h"
#include <stdlib.h>
#include <string.h>

enum _XfconfPropertyType
{
	XFCONF_PROPERTY_NONE = 0, // After property creation
	XFCONF_PROPERTY_STRING,

	XFCONF_PROPERTY_TYPE_COUNT
};
typedef enum _XfconfPropertyType XfconfPropertyType;

typedef struct _XfconfProperty XfconfProperty;
struct _XfconfProperty
{
	GObject parent;

	const gchar *property_name;

	XfconfPropertyType type;
	gchar* str_value;

	// pointer to next property of the linked list
	XfconfProperty *next;
};


struct _XfconfChannel
{
	GObject parent;

	const gchar *channel_name;

	// pointer to the start of the linked list of properties
	XfconfProperty* first_property;

	// pointer to next channel of the linked list
	XfconfChannel *next;
};

static XfconfChannel* first = NULL;

// return null for error (e.g. null pointer for channel_name)
// does NOT add the channel to the list.
// for this call priv_xfconf_add_channel().
static XfconfChannel *priv_xfconf_new_channel(const gchar *channel_name);

// return null if not found
static XfconfChannel *priv_xfconf_find_channel(const gchar *channel_name);

// return true if added.
static gboolean priv_xfconf_add_channel(XfconfChannel *channel);

// channel: address of a channel pointer
//          double pointer is used to reset the pointer to NULL.
// note: The next channel is NOT freed! If next channel pointer is != NULL
//       then its not freed. Currently no problem because currently
//       this function is only used to free a new failed channel which
//       has a null pointer for next.
static void priv_xfconf_free_channel(XfconfChannel **channel);


// --- functions for properties ---

static XfconfProperty *priv_xfconf_get_property(XfconfChannel* channel, const gchar *property_name);

static XfconfProperty *priv_xfconf_new_property(const gchar *property_name);
// return null if not found
static XfconfProperty *priv_xfconf_find_property(XfconfChannel *channel, const gchar *property_name);
static gboolean priv_xfconf_add_property(XfconfChannel *channel, XfconfProperty *property);

// property: address of a property pointer
//          double pointer is used to reset the pointer to NULL.
// note: The next property is NOT freed! If next property pointer is != NULL
//       then its not freed.
//       If next is not null then maybe this pointer should be backup to
//       free it after this function.
static void priv_xfconf_free_property(XfconfProperty **property);

///////////////////////////

gboolean xfconf_init(GError **error)
{
	g_warning("TODO: xfconf_init()");
	return TRUE;
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
	XfconfChannel *channel = NULL;

	if (!channel_name) {
		g_warning("null ptr for channel_name (get)");
		return NULL;
	}
	channel = priv_xfconf_find_channel(channel_name);
	if (channel) {
		return channel;
	}
	// --> channel not found --> create new one
	channel = priv_xfconf_new_channel(channel_name);
	if (!channel) {
		g_warning("Create new channel '%s' failed. (get)", channel_name);
		return NULL;
	}
	if (!priv_xfconf_add_channel(channel)) {
		g_warning("Add new channel '%s' failed. (get)", channel_name);
		priv_xfconf_free_channel(&channel);
		return NULL;
	}
	g_warning("Create new channel '%s'. (get)", channel_name);
	return channel;
}

gboolean xfconf_channel_set_string(XfconfChannel *channel,
                                   const gchar *property_name,
                                   const gchar *value)
{
	XfconfProperty *property = NULL;
	int prop_str_len = 0;
	int param_str_len = 0;
	
	if (!channel) {
		g_warning("null ptr for channel (set string)");
		return FALSE;
	}
	if (!property_name) {
		g_warning("null ptr for property_name (set string)");
		return FALSE;
	}
	if (!value) {
		g_warning("null ptr for value (set string)");
		return FALSE;
	}
	property = priv_xfconf_get_property(channel, property_name);
	if (!property) {
		g_warning("Can't get property (set string)");
		return FALSE;
	}
	if (property->type != XFCONF_PROPERTY_NONE &&
			property->type != XFCONF_PROPERTY_STRING) {
		g_warning("Wrong type. Set string failed: ch '%s', prop '%s', value '%s' (set string)", channel->channel_name, property_name, value);
		return FALSE;
	}
	param_str_len = strlen(value);
	prop_str_len = property->str_value ? strlen(property->str_value) : -1;
	if (param_str_len != prop_str_len) {
		free(property->str_value);
		property->str_value = calloc(param_str_len + 1, sizeof(gchar));
	}
	strcpy(property->str_value, value);
	property->type = XFCONF_PROPERTY_STRING;
	g_warning("Set string ok: ch '%s', prop '%s', value '%s' (set string)",
		channel->channel_name, property_name, value);
	return TRUE;
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
	gboolean has_property = FALSE;
	if (!channel) {
		g_warning("null ptr for channel (has_property)");
		return FALSE;
	}
	// return null if not found
	has_property = (priv_xfconf_find_property(channel, property) != NULL);
	g_warning("xfconf_channel_has_property() for '%s' '%s': %s", channel->channel_name, property, has_property ? "true" : "false");
	return has_property;
}

void xfconf_channel_reset_property(XfconfChannel *channel,
                                   const gchar *property_base,
                                   gboolean recursive)
{
	g_warning("TODO: xfconf_channel_reset_property()");
}

gboolean xfconf_channel_get_property(XfconfChannel *channel,
                                     const gchar *property_name,
                                     GValue *value)
{
	XfconfProperty *property = NULL;
	if (!channel) {
		g_warning("null ptr for channel (get property)");
		return FALSE;
	}
	if (!value) {
		g_warning("null ptr for value (get property)");
		return FALSE;
	}
	property = priv_xfconf_find_property(channel, property_name);
	if (!property) {
		g_warning("xfconf_channel_get_property() for '%s' '%s' failed. property not exist",
				channel->channel_name, property_name);
		return FALSE;
	}
	switch (property->type) {
		case XFCONF_PROPERTY_NONE:
			g_warning("type \"NONE\" is not supported (get property)");
			return FALSE;
		case XFCONF_PROPERTY_STRING:
			// TODO: or non static version?
			//g_value_set_string(value, property->str_value);
			g_value_set_static_string(value, property->str_value);
			break;
		case XFCONF_PROPERTY_TYPE_COUNT:
			g_warning("type \"TYPE_COUNT\" is not supported (get property)");
			return FALSE;
	}
	g_warning("xfconf_channel_get_property() for '%s' '%s': %s", channel->channel_name, property_name, property ? "true" : "false");
	return TRUE;
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

////////////////////////////////////////

static XfconfChannel *priv_xfconf_new_channel(const gchar *channel_name)
{
	XfconfChannel* channel = NULL;
	int len = 0;
	gchar* new_channel_name = NULL;

	if (!channel_name) {
		g_warning("null ptr for channel_name (new)");
		return NULL;
	}
	channel = calloc(1, sizeof(XfconfChannel));
	len = strlen(channel_name);
	new_channel_name = calloc(len + 1, sizeof(gchar));
	strcpy(new_channel_name, channel_name);
	channel->channel_name = new_channel_name;
	return channel;
}

static XfconfChannel *priv_xfconf_find_channel(const gchar *channel_name)
{
	if (!channel_name) {
		g_warning("null ptr for channel_name (find)");
		return NULL;
	}
	for (XfconfChannel* cur = first; cur; cur = cur->next) {
		if (cur->channel_name && strcmp(cur->channel_name, channel_name) == 0) {
			return cur;
		}
	}
	g_warning("Can't find channel '%s'", channel_name);
	return NULL;
}

static gboolean priv_xfconf_add_channel(XfconfChannel *channel)
{
	XfconfChannel** cur = &first;
	int i = 0;
	if (!channel) {
		g_warning("null ptr for channel (add)");
		return FALSE;
	}
	for (; *cur; cur = &(*cur)->next, ++i) {
		if ((*cur)->channel_name && strcmp((*cur)->channel_name, channel->channel_name) == 0) {
			g_warning("Can't add channel to list. Channel '%s' already exist (add)", channel->channel_name);
			return FALSE;
		}
	}
	*cur = channel;
	g_warning("Add channel '%s' to list at index position %d (add)", channel->channel_name, i);
	return TRUE;
}

static void priv_xfconf_free_channel(XfconfChannel **channel)
{
	XfconfProperty* next_property = NULL;
	if (!channel) {
		g_warning("double pointer channel is null (free)");
		return;
	}
	if (!*channel) {
		g_warning("pointer channel is null (free)");
		return;
	}

	// free channel_name
	// (gchar*) cast to remote const --> no warning for free();
	free((gchar*)(*channel)->channel_name);
	(*channel)->channel_name = NULL;

	// free property list
	for (XfconfProperty* cur = (*channel)->first_property; cur; cur = next_property) {
		// backup next pointer because after free the current property the
		// access per cur->next is not allowed (memory already freed).
		next_property = cur->next;
		priv_xfconf_free_property(&cur);
	}

	// free channel
	free(*channel);
	*channel = NULL;
}

// --- functions for properties ---

static XfconfProperty *priv_xfconf_get_property(XfconfChannel* channel, const gchar *property_name)
{
	XfconfProperty *property = NULL;
	if (!channel) {
		g_warning("channel is null (get property)");
		return NULL;
	}
	if (!property_name) {
		g_warning("property_name is null (get property)");
		return NULL;
	}
	property = priv_xfconf_find_property(channel, property_name);
	if (property) {
		return property;
	}
	// --> property not found --> create new one
	property = priv_xfconf_new_property(property_name);
	if (!property) {
		g_warning("Create new property '%s' failed. (get property)", property_name);
		return NULL;
	}
	if (!priv_xfconf_add_property(channel, property)) {
		g_warning("Add new property '%s' failed. (get property)", property_name);
		priv_xfconf_free_property(&property);
		return NULL;
	}
	g_warning("Create new property '%s' for channel '%s'. (get property)", property_name, channel->channel_name);
	return property;
}

static XfconfProperty *priv_xfconf_new_property(const gchar *property_name)
{
	XfconfProperty* property = NULL;
	int len = 0;
	gchar* new_property_name = NULL;

	if (!property_name) {
		g_warning("null ptr for property_name (new property)");
		return NULL;
	}
	property = calloc(1, sizeof(XfconfProperty));
	len = strlen(property_name);
	new_property_name = calloc(len + 1, sizeof(gchar));
	strcpy(new_property_name, property_name);
	property->property_name = new_property_name;
	return property;
}

static XfconfProperty *priv_xfconf_find_property(XfconfChannel *channel, const gchar *property_name)
{
	if (!channel) {
		g_warning("channel is null (find property)");
		return NULL;
	}
	if (!property_name) {
		g_warning("property_name is null (find property)");
		return NULL;
	}
	for (XfconfProperty* cur = channel->first_property; cur; cur = cur->next) {
		if (cur->property_name && strcmp(cur->property_name, property_name) == 0) {
			return cur;
		}
	}
	g_warning("Can't find property '%s' for channel '%s'", property_name, channel->channel_name);
	return NULL;
}

static gboolean priv_xfconf_add_property(XfconfChannel *channel, XfconfProperty *property)
{
	XfconfProperty **cur = NULL;
	int i = 0;
	if (!channel) {
		g_warning("channel is null (add property)");
		return FALSE;
	}
	if (!property) {
		g_warning("property is null (add property)");
		return FALSE;
	}
	for (cur = &channel->first_property; *cur; cur = &(*cur)->next, ++i) {
		if ((*cur)->property_name && strcmp((*cur)->property_name, property->property_name) == 0) {
			g_warning("Can't add property to list. Channel '%s' already exist (add property)", property->property_name);
			return FALSE;
		}
	}
	*cur = property;
	g_warning("Add property '%s' to list at index position %d (add property)", property->property_name, i);
	return TRUE;
}

static void priv_xfconf_free_property(XfconfProperty **property)
{
	if (!property) {
		g_warning("double pointer property is null (free property)");
		return;
	}
	if (!*property) {
		g_warning("pointer property is null (free property)");
		return;
	}

	// free property_name
	// (gchar*) cast to remote const --> no warning for free();
	free((gchar*)(*property)->property_name);
	(*property)->property_name = NULL;

	// free property
	free(*property);
	*property = NULL;
}

