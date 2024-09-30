
#include "xfconf_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <glib/garray.h>
#include <gobject/gvaluecollector.h>

#define XFCONF_TYPE_UINT16 (xfconf_uint16_get_type())
#define XFCONF_TYPE_INT16 (xfconf_int16_get_type())

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

	GValue *value;

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

// --- functions for value ---

// create a new if necessary or otherwise update it
static gboolean priv_xfconf_set_property_value(XfconfProperty *property, const GValue* value);

// modified versions from xfconf/xfconf-cache.c
static void priv_xfconf_cache_item_new(XfconfProperty* item, const GValue *value, gboolean steal);
static gboolean priv_xfconf_cache_item_update(XfconfProperty *item, const GValue *value);
static void priv_xfconf_cache_item_free(XfconfProperty *item);

// from common/xfconf-gvaluefuncs.c
static void priv_xfonf_free_array_elem_val(gpointer data);
static GPtrArray *priv_xfconf_dup_value_array(GPtrArray *arr);
static gboolean priv_xfconf_gvalue_is_equal(const GValue *value1, const GValue *value2);


// from xfconf/xfconf-types.h and common/xfconf-types.c
static GType xfconf_uint16_get_type(void) G_GNUC_CONST;

static guint16 xfconf_g_value_get_uint16(const GValue *value);
static void xfconf_g_value_set_uint16(GValue *value,
                                      guint16 v_uint16);

static GType xfconf_int16_get_type(void) G_GNUC_CONST;

static gint16 xfconf_g_value_get_int16(const GValue *value);
static void xfconf_g_value_set_int16(GValue *value,
                                     gint16 v_int16);


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
	GValue val = G_VALUE_INIT;
	gboolean ret;
	
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

	g_value_init(&val, G_TYPE_STRING);
	g_value_set_string(&val, value);
	//g_value_set_static_string(&val, value);

	ret = priv_xfconf_set_property_value(property, &val);

	g_value_unset(&val);

	g_warning("Set string: ch '%s', prop '%s', value '%s', rv %s (set string)",
		channel->channel_name, property_name, value, ret ? "true" : "false");
	return ret;
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
	GValue val1 = G_VALUE_INIT;
	gboolean ret = FALSE;

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

	if (property->value) {
		ret = TRUE;
		val1 = *property->value;
	}

    //ret = xfconf_channel_get_internal(channel, property_name, &val1);

    if (ret) {
        if (G_VALUE_TYPE(value) != G_TYPE_INVALID
            && G_VALUE_TYPE(value) != G_VALUE_TYPE(&val1))
        {
            /* caller wants to convert the returned value into a diff type */

            if (G_VALUE_TYPE(&val1) == G_TYPE_PTR_ARRAY) {
                /* we got an array back, so let's convert each item in
                 * the array to the target type */
                //TODO: add support for xfconf_transform_array()
                GPtrArray *arr = NULL;
                g_warning("TODO: add xfconf_transform_array() support");
                //GPtrArray *arr = xfconf_transform_array(g_value_get_boxed(&val1),
                //                                        G_VALUE_TYPE(value));

                if (arr) {
                    g_value_unset(value);
                    g_value_init(value, G_TYPE_PTR_ARRAY);
                    g_value_take_boxed(value, arr);
                } else {
                    ret = FALSE;
                }
            } else {
                ret = g_value_transform(&val1, value);
                if (!ret) {
                    g_warning("Unable to convert property \"%s\" from type \"%s\" to type \"%s\"",
                              property_name, G_VALUE_TYPE_NAME(&val1),
                              G_VALUE_TYPE_NAME(value));
                }
            }
        } else {
            /* either the caller wants the native type, or specified the
             * native type to convert to */
            if (G_VALUE_TYPE(value) == G_VALUE_TYPE(&val1)) {
                g_value_unset(value);
            }
            g_value_copy(&val1, g_value_init(value, G_VALUE_TYPE(&val1)));
            ret = TRUE;
        }
    }

    if (G_VALUE_TYPE(&val1)) {
        g_value_unset(&val1);
    }

    g_warning("xfconf_channel_get_property() for '%s' '%s': %s", channel->channel_name, property_name, (property && ret) ? "true" : "false");
    return ret;
}

gboolean xfconf_channel_set_property(XfconfChannel *channel,
                                     const gchar *property_name,
                                     const GValue *value)
{
	XfconfProperty *property = NULL;
	GValue val = G_VALUE_INIT;
	gboolean ret;
	gchar *str_val;
	
	if (!channel) {
		g_warning("null ptr for channel (set property)");
		return FALSE;
	}
	if (!property_name) {
		g_warning("null ptr for property_name (set property)");
		return FALSE;
	}
	if (!value) {
		g_warning("null ptr for value (set property)");
		return FALSE;
	}
	property = priv_xfconf_get_property(channel, property_name);
	if (!property) {
		g_warning("Can't get property (set property)");
		return FALSE;
	}

	g_value_init(&val, G_VALUE_TYPE(value));
	g_value_copy(value, &val);
	ret = priv_xfconf_set_property_value(property, &val);
	//ret = FALSE;
	g_value_unset(&val);
	//ret = priv_xfconf_set_property_value(property, value);

	//str_val = g_strdup_value_contents(value);
	//g_print ("gvalue: %s\n", str_val);
	//g_warning("xfconf_channel_set_property() %s %s %s: rv %s", channel->channel_name, property_name, str_val, ret ? "true" : "false");
	//free(str_val);
	return ret;
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

static gboolean priv_xfconf_set_property_value(XfconfProperty *property, const GValue* value)
{
	gboolean rv = TRUE;
	if (!property) {
		g_warning("property is null (set property value)");
		return FALSE;
	}
	if (value) {
		// value from parameter is available
#if 0
		if (property->value) {
			// --> value available --> only update it
			rv = priv_xfconf_cache_item_update(property, value);
		}
		else {
			// --> not available --> create a new one
			priv_xfconf_cache_item_new(property, value, FALSE);
		}
#else
		// always use update
		rv = priv_xfconf_cache_item_update(property, value);
#endif
	}
	else {
		g_warning("free property because of null value (set property value)");
		// value not available (null pointer) --> delete also the value from property
		priv_xfconf_cache_item_free(property);
	}
	return rv;
}

static void priv_xfconf_cache_item_new(XfconfProperty* item, const GValue *value, gboolean steal)
{
	if (G_LIKELY(steal) || value == NULL) {
		item->value = (GValue *)value;
	} else {

		item->value = g_new0(GValue, 1);
		g_value_init(item->value, G_VALUE_TYPE(value));
		/* We need to dup the array */
		if (G_VALUE_TYPE(value) == G_TYPE_PTR_ARRAY) {
			GPtrArray *arr = priv_xfconf_dup_value_array(g_value_get_boxed(value));
			g_value_take_boxed(item->value, arr);
		} else {
			g_value_copy(value, item->value);
		}
	}
}

static gboolean priv_xfconf_cache_item_update(XfconfProperty *item, const GValue *value)
{
	if (value && priv_xfconf_gvalue_is_equal(item->value, value)) {
		return FALSE;
	}

	if (value) {
		if (item->value == NULL) {
			item->value = g_new0(GValue, 1);
		} else {
			g_value_unset(item->value);
		}

		g_value_init(item->value, G_VALUE_TYPE(value));

		/* We need to dup the array */
		if (G_VALUE_TYPE(value) == G_TYPE_PTR_ARRAY) {
			g_warning("*** array ***");
			GPtrArray *arr = priv_xfconf_dup_value_array(g_value_get_boxed(value));
			g_value_take_boxed(item->value, arr);
		} else {
			g_warning("*** normal copy ***");
			g_value_copy(value, item->value);
		}
		return TRUE;
	}

	return FALSE;
}

static void priv_xfconf_cache_item_free(XfconfProperty *item)
{
    g_return_if_fail(item);

    if (item->value != NULL) {
        g_value_unset(item->value);
        g_free(item->value);
	item->value = NULL;
    }
    //g_slice_free(XfconfCacheItem, item); // don't use this for this XfconfProperty implementation
}

static void priv_xfonf_free_array_elem_val(gpointer data)
{
	GValue *val = (GValue *)data;
	g_value_unset(val);
	g_free(val);
}

static GPtrArray *priv_xfconf_dup_value_array(GPtrArray *arr)
{

	GPtrArray *retArr;
	uint i;

	retArr = g_ptr_array_new_full(arr->len, priv_xfonf_free_array_elem_val);

	for (i = 0; i < arr->len; i++) {
		GValue *v, *vi;
		v = g_new0(GValue, 1);
		vi = g_ptr_array_index(arr, i);
		g_value_init(v, G_VALUE_TYPE(vi));
		g_value_copy(vi, v);
		g_ptr_array_add(retArr, v);
	}

	return retArr;
}

static gboolean
priv_xfconf_gvalue_is_equal(const GValue *value1,
                            const GValue *value2)
{
    if (G_UNLIKELY(!value1 && !value2)) {
        return TRUE;
    }
    if (G_UNLIKELY(!value1 || !value2)) {
        return FALSE;
    }
    if (G_VALUE_TYPE(value1) != G_VALUE_TYPE(value2)) {
        return FALSE;
    }
    if (G_VALUE_TYPE(value1) == G_TYPE_INVALID || G_VALUE_TYPE(value1) == G_TYPE_NONE) {
        return TRUE;
    }

    switch (G_VALUE_TYPE(value1)) {
#define HANDLE_CMP_GV(TYPE, getter) \
    case G_TYPE_##TYPE: \
        return g_value_get_##getter(value1) == g_value_get_##getter(value2)

        HANDLE_CMP_GV(CHAR, schar);
        HANDLE_CMP_GV(UCHAR, uchar);
        HANDLE_CMP_GV(BOOLEAN, boolean);
        HANDLE_CMP_GV(INT, int);
        HANDLE_CMP_GV(UINT, uint);
        HANDLE_CMP_GV(INT64, int64);
        HANDLE_CMP_GV(UINT64, uint64);
        HANDLE_CMP_GV(FLOAT, float);
        HANDLE_CMP_GV(DOUBLE, double);

        case G_TYPE_STRING:
            return !g_strcmp0(g_value_get_string(value1), g_value_get_string(value2));

        default:
            if (G_VALUE_TYPE(value1) == XFCONF_TYPE_INT16) {
                return xfconf_g_value_get_int16(value1) == xfconf_g_value_get_uint16(value2);
            } else if (G_VALUE_TYPE(value1) == XFCONF_TYPE_UINT16) {
                return xfconf_g_value_get_uint16(value1) == xfconf_g_value_get_uint16(value2);
            }
            break;
#undef HANDLE_CMP_GV
    }

    return FALSE;
}

static void
gvalue_from_short(const GValue *src_value,
                  GValue *dest_value)
{
#define HANDLE_TYPE(gtype_s, getter) \
    case G_TYPE_##gtype_s: \
        dest = (guint64)g_value_get_##getter(src_value); \
        break;

    guint64 dest; /* use larger type so we can handle int16 & uint16 */

    switch (G_VALUE_TYPE(src_value)) {
        case G_TYPE_STRING:
            dest = atoi(g_value_get_string(src_value));
            break;
        case G_TYPE_BOOLEAN:
            dest = g_value_get_boolean(src_value) == TRUE ? 1 : 0;
            break;
            HANDLE_TYPE(CHAR, schar)
            HANDLE_TYPE(UCHAR, uchar)
            HANDLE_TYPE(INT, int)
            HANDLE_TYPE(UINT, uint)
            HANDLE_TYPE(LONG, long)
            HANDLE_TYPE(ULONG, ulong)
            HANDLE_TYPE(INT64, int64)
            HANDLE_TYPE(UINT64, uint64)
            HANDLE_TYPE(ENUM, enum)
            HANDLE_TYPE(FLAGS, flags)
            HANDLE_TYPE(FLOAT, float)
            HANDLE_TYPE(DOUBLE, double)
        default:
            return;
    }

    if (G_VALUE_TYPE(dest_value) == XFCONF_TYPE_UINT16) {
        if (dest > USHRT_MAX) {
            g_warning("Converting type \"%s\" to \"%s\" results in overflow",
                      G_VALUE_TYPE_NAME(src_value),
                      G_VALUE_TYPE_NAME(dest_value));
        }
        xfconf_g_value_set_uint16(dest_value, (guint16)dest);
    } else if (G_VALUE_TYPE(dest_value) == XFCONF_TYPE_INT16) {
        if (dest > (guint64)SHRT_MAX || dest < (guint64)SHRT_MIN) {
            g_warning("Converting type \"%s\" to \"%s\" results in overflow",
                      G_VALUE_TYPE_NAME(src_value),
                      G_VALUE_TYPE_NAME(dest_value));
        }
        xfconf_g_value_set_int16(dest_value, (gint16)dest);
    }
#undef HANDLE_TYPE
}

static void
short_from_gvalue(const GValue *src_value,
                  GValue *dest_value)
{
#define HANDLE_TYPE(gtype_s, setter) \
    case G_TYPE_##gtype_s: \
        g_value_set_##setter(dest_value, src); \
        break;

    guint16 src;
    gboolean is_signed = FALSE;

    if (G_VALUE_TYPE(src_value) == XFCONF_TYPE_UINT16) {
        src = xfconf_g_value_get_uint16(src_value);
    } else if (G_VALUE_TYPE(src_value) == XFCONF_TYPE_INT16) {
        src = xfconf_g_value_get_int16(src_value);
        is_signed = TRUE;
    } else {
        return;
    }

    switch (G_VALUE_TYPE(dest_value)) {
        case G_TYPE_STRING: {
            gchar *str = g_strdup_printf(is_signed ? "%d" : "%u",
                                         is_signed ? (gint16)src : src);
            g_value_set_string(dest_value, str);
            g_free(str);
            break;
        }
        case G_TYPE_BOOLEAN:
            g_value_set_boolean(dest_value, src ? TRUE : FALSE);
            break;
            HANDLE_TYPE(CHAR, schar)
            HANDLE_TYPE(UCHAR, uchar)
            HANDLE_TYPE(INT, int)
            HANDLE_TYPE(UINT, uint)
            HANDLE_TYPE(LONG, long)
            HANDLE_TYPE(ULONG, ulong)
            HANDLE_TYPE(INT64, int64)
            HANDLE_TYPE(UINT64, uint64)
            HANDLE_TYPE(ENUM, enum)
            HANDLE_TYPE(FLAGS, flags)
            HANDLE_TYPE(FLOAT, float)
            HANDLE_TYPE(DOUBLE, double)
        default:
            return;
    }
#undef HANDLE_TYPE
}

static void
register_transforms(GType gtype)
{
    GType types[] = {
        G_TYPE_CHAR, G_TYPE_UCHAR, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_UINT,
        G_TYPE_LONG, G_TYPE_ULONG, G_TYPE_INT64, G_TYPE_UINT64,
        G_TYPE_ENUM, G_TYPE_FLAGS, G_TYPE_FLOAT, G_TYPE_DOUBLE,
        G_TYPE_STRING, G_TYPE_INVALID
    };
    gint i;

    for (i = 0; types[i] != G_TYPE_INVALID; ++i) {
        g_value_register_transform_func(gtype, types[i], gvalue_from_short);
        g_value_register_transform_func(types[i], gtype, short_from_gvalue);
    }
}

static void
ushort_value_init(GValue *value)
{
    value->data[0].v_int = 0;
}

static void
ushort_value_copy(const GValue *src_value,
                  GValue *dest_value)
{
    dest_value->data[0].v_int = src_value->data[0].v_int;
}

static gchar *
ushort_value_collect(GValue *value,
                     guint n_collect_values,
                     GTypeCValue *collect_values,
                     guint collect_flags)
{
    value->data[0].v_int = collect_values[0].v_int;
    return NULL;
}

static gchar *
ushort_value_lcopy(const GValue *value,
                   guint n_collect_values,
                   GTypeCValue *collect_values,
                   guint collect_flags)
{
    guint16 *uint16_p = collect_values[0].v_pointer;

    if (!uint16_p) {
        return g_strdup_printf("value location for `%s' passed as NULL",
                               G_VALUE_TYPE_NAME(value));
    }

    *uint16_p = value->data[0].v_int;

    return NULL;
}

static GType
xfconf_uint16_get_type(void)
{
    static GType uint16_type = 0;
    GTypeFundamentalInfo finfo = { 0 };
    GTypeInfo info = { 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL };
    static const GTypeValueTable value_table = {
        ushort_value_init,
        NULL,
        ushort_value_copy,
        NULL,
        (gchar *)"i",
        ushort_value_collect,
        (gchar *)"p",
        ushort_value_lcopy
    };

    if (!uint16_type) {
        info.value_table = &value_table;
        uint16_type = g_type_register_fundamental(g_type_fundamental_next(),
                                                  "XfconfUint16", &info,
                                                  &finfo, 0);
        register_transforms(uint16_type);
    }

    return uint16_type;
}

/**
 * xfconf_g_value_get_uint16:
 * @value: A #GValue.
 *
 * Retrieves a 16-bit unsigned value from @value.
 *
 * Returns: A guint16.
 **/
static guint16
xfconf_g_value_get_uint16(const GValue *value)
{
    g_return_val_if_fail(G_VALUE_HOLDS(value, XFCONF_TYPE_UINT16), 0);
    return (guint16)value->data[0].v_int;
}

/**
 * xfconf_g_value_set_uint16:
 * @value: A #GValue.
 * @v_uint16: A guint16.
 *
 * Sets @value using an unsigned 16-bit integer.
 **/
static void
xfconf_g_value_set_uint16(GValue *value,
                          guint16 v_uint16)
{
    g_return_if_fail(G_VALUE_HOLDS(value, XFCONF_TYPE_UINT16));
    value->data[0].v_int = v_uint16;
}

static GType
xfconf_int16_get_type(void)
{
    static GType int16_type = 0;
    GTypeFundamentalInfo finfo = { 0 };
    GTypeInfo info = { 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL };
    static const GTypeValueTable value_table = {
        ushort_value_init,
        NULL,
        ushort_value_copy,
        NULL,
        (gchar *)"i",
        ushort_value_collect,
        (gchar *)"p",
        ushort_value_lcopy
    };

    if (!int16_type) {
        info.value_table = &value_table;
        int16_type = g_type_register_fundamental(g_type_fundamental_next(),
                                                 "XfconfInt16", &info,
                                                 &finfo, 0);
        register_transforms(int16_type);
    }

    return int16_type;
}

/**
 * xfconf_g_value_get_int16:
 * @value: A #GValue.
 *
 * Retrieves a 16-bit signed value from @value.
 *
 * Returns: A gint16.
 **/
static gint16
xfconf_g_value_get_int16(const GValue *value)
{
    g_return_val_if_fail(G_VALUE_HOLDS(value, XFCONF_TYPE_INT16), 0);
    return (gint16)value->data[0].v_int;
}

/**
 * xfconf_g_value_set_int16:
 * @value: A #GValue.
 * @v_int16: A gint16.
 *
 * Sets @value using a signed 16-bit integer.
 **/
static void
xfconf_g_value_set_int16(GValue *value,
                         gint16 v_int16)
{
    g_return_if_fail(G_VALUE_HOLDS(value, XFCONF_TYPE_INT16));
    value->data[0].v_int = v_int16;
}

