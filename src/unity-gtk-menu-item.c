#include "unity-gtk-menu-item-private.h"

G_DEFINE_TYPE (UnityGtkMenuItem, unity_gtk_menu_item, G_TYPE_OBJECT);

enum
{
  MENU_ITEM_PROP_0,
  MENU_ITEM_PROP_MENU_ITEM,
  MENU_ITEM_PROP_PARENT_SHELL,
  MENU_ITEM_PROP_CHILD_SHELL,
  MENU_ITEM_PROP_ITEM_INDEX,
  MENU_ITEM_N_PROPERTIES
};

static GParamSpec *menu_item_properties[MENU_ITEM_N_PROPERTIES] = { NULL };

static void
unity_gtk_menu_item_handle_item_notify (GObject    *object,
                                        GParamSpec *pspec,
                                        gpointer    user_data)
{
  UnityGtkMenuItem *item;
  UnityGtkMenuShell *parent_shell;
  GObject *menu_item;

  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (user_data));

  item = UNITY_GTK_MENU_ITEM (user_data);
  parent_shell = item->parent_shell;
  menu_item = G_OBJECT (item->menu_item);

  g_return_if_fail (parent_shell != NULL);
  g_warn_if_fail (object == menu_item);

  unity_gtk_menu_shell_handle_item_notify (parent_shell, item, pspec);
}

static void
unity_gtk_menu_item_set_parent_shell (UnityGtkMenuItem  *item,
                                      UnityGtkMenuShell *parent_shell)
{
  UnityGtkMenuShell *old_parent_shell;

  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (item));

  old_parent_shell = item->parent_shell;

  if (parent_shell != old_parent_shell)
    {
      if (old_parent_shell != NULL)
        {
          item->parent_shell = NULL;
          g_object_unref (old_parent_shell);
        }

      if (parent_shell != NULL)
        item->parent_shell = g_object_ref (parent_shell);
    }
}

static void
unity_gtk_menu_item_dispose (GObject *object)
{
  UnityGtkMenuItem *item;

  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (object));

  item = UNITY_GTK_MENU_ITEM (object);

  unity_gtk_menu_item_set_parent_shell (item, NULL);
  unity_gtk_menu_item_set_menu_item (item, NULL);

  G_OBJECT_CLASS (unity_gtk_menu_item_parent_class)->dispose (object);
}

static void
unity_gtk_menu_item_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  UnityGtkMenuItem *self;

  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (object));

  self = UNITY_GTK_MENU_ITEM (object);

  switch (property_id)
    {
    case MENU_ITEM_PROP_MENU_ITEM:
      g_value_set_pointer (value, unity_gtk_menu_item_get_menu_item (self));
      break;

    case MENU_ITEM_PROP_PARENT_SHELL:
      g_value_set_object (value, self->parent_shell);
      break;

    case MENU_ITEM_PROP_CHILD_SHELL:
      g_value_set_object (value, unity_gtk_menu_item_get_child_shell (self));
      break;

    case MENU_ITEM_PROP_ITEM_INDEX:
      g_value_set_uint (value, unity_gtk_menu_item_get_item_index (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
unity_gtk_menu_item_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  UnityGtkMenuItem *self;

  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (object));

  self = UNITY_GTK_MENU_ITEM (object);

  switch (property_id)
    {
    case MENU_ITEM_PROP_MENU_ITEM:
      unity_gtk_menu_item_set_menu_item (self, g_value_get_pointer (value));
      break;

    case MENU_ITEM_PROP_PARENT_SHELL:
      unity_gtk_menu_item_set_parent_shell (self, g_value_get_object (value));
      break;

    case MENU_ITEM_PROP_ITEM_INDEX:
      unity_gtk_menu_item_set_item_index (self, g_value_get_uint (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
unity_gtk_menu_item_class_init (UnityGtkMenuItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = unity_gtk_menu_item_dispose;
  object_class->get_property = unity_gtk_menu_item_get_property;
  object_class->set_property = unity_gtk_menu_item_set_property;

  menu_item_properties[MENU_ITEM_PROP_MENU_ITEM] = g_param_spec_pointer ("menu-item",
                                                                         "Menu item",
                                                                         "Menu item",
                                                                         G_PARAM_READWRITE);
  menu_item_properties[MENU_ITEM_PROP_PARENT_SHELL] = g_param_spec_object ("parent-shell",
                                                                           "Parent shell",
                                                                           "Parent shell",
                                                                           UNITY_GTK_TYPE_MENU_SHELL,
                                                                           G_PARAM_READWRITE);
  menu_item_properties[MENU_ITEM_PROP_CHILD_SHELL] = g_param_spec_object ("child-shell",
                                                                          "Child shell",
                                                                          "Child shell",
                                                                          UNITY_GTK_TYPE_MENU_SHELL,
                                                                          G_PARAM_READABLE);
  menu_item_properties[MENU_ITEM_PROP_ITEM_INDEX] = g_param_spec_uint ("item-index",
                                                                       "Item index",
                                                                       "Item index",
                                                                       0,
                                                                       G_MAXUINT,
                                                                       0,
                                                                       G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, MENU_ITEM_N_PROPERTIES, menu_item_properties);
}

static void
unity_gtk_menu_item_init (UnityGtkMenuItem *self)
{
}

UnityGtkMenuItem *
unity_gtk_menu_item_new (GtkMenuItem       *menu_item,
                         UnityGtkMenuShell *parent_shell,
                         guint              item_index)
{
  return g_object_new (UNITY_GTK_TYPE_MENU_ITEM,
                       "menu-item", menu_item,
                       "parent-shell", parent_shell,
                       "item-index", item_index,
                       NULL);
}

GtkMenuItem *
unity_gtk_menu_item_get_menu_item (UnityGtkMenuItem *item)
{
  g_return_val_if_fail (UNITY_GTK_IS_MENU_ITEM (item), NULL);

  return item->menu_item;
}

void
unity_gtk_menu_item_set_menu_item (UnityGtkMenuItem *item,
                                   GtkMenuItem      *menu_item)
{
  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (item));

  if (menu_item != item->menu_item)
    {
      UnityGtkMenuShell *child_shell = item->child_shell;

      if (item->menu_item_notify_handler_id)
        {
          g_assert (item->menu_item != NULL);
          g_signal_handler_disconnect (item->menu_item, item->menu_item_notify_handler_id);
          item->menu_item_notify_handler_id = 0;
        }

      if (child_shell != NULL)
        {
          g_assert (item->child_shell_valid);
          item->child_shell = NULL;
          g_object_unref (child_shell);
        }

      item->child_shell_valid = FALSE;
      item->menu_item = menu_item;

      if (menu_item != NULL)
        item->menu_item_notify_handler_id = g_signal_connect (menu_item, "notify", G_CALLBACK (unity_gtk_menu_item_handle_item_notify), item);
    }
}

UnityGtkMenuShell *
unity_gtk_menu_item_get_child_shell (UnityGtkMenuItem *item)
{
  g_return_val_if_fail (UNITY_GTK_IS_MENU_ITEM (item), NULL);

  if (!item->child_shell_valid)
    {
      GtkMenuItem *menu_item = item->menu_item;
      UnityGtkMenuShell *child_shell = item->child_shell;

      if (child_shell != NULL)
        {
          g_warn_if_reached ();
          item->child_shell = NULL;
          g_object_unref (child_shell);
        }

      if (menu_item != NULL)
        {
          GtkWidget *submenu = gtk_menu_item_get_submenu (menu_item);

          if (submenu != NULL)
            item->child_shell = unity_gtk_menu_shell_new (GTK_MENU_SHELL (submenu));
        }

      item->child_shell_valid = TRUE;
    }

  return item->child_shell;
}

guint
unity_gtk_menu_item_get_item_index (UnityGtkMenuItem *item)
{
  g_return_val_if_fail (UNITY_GTK_IS_MENU_ITEM (item), 0);

  return item->item_index;
}

void
unity_gtk_menu_item_set_item_index (UnityGtkMenuItem *item,
                                    guint             item_index)
{
  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (item));

  item->item_index = item_index;
}

const gchar *
unity_gtk_menu_item_get_label (UnityGtkMenuItem *item)
{
  const gchar *label;

  g_return_val_if_fail (UNITY_GTK_IS_MENU_ITEM (item), NULL);
  g_return_val_if_fail (item->menu_item != NULL, NULL);

  label = gtk_menu_item_get_label (item->menu_item);

  return label != NULL && label[0] != '\0' ? label : NULL;
}

gboolean
unity_gtk_menu_item_is_visible (UnityGtkMenuItem *item)
{
  g_return_val_if_fail (UNITY_GTK_IS_MENU_ITEM (item), FALSE);

  return item->menu_item != NULL && !GTK_IS_TEAROFF_MENU_ITEM (item->menu_item) && gtk_widget_get_visible (GTK_WIDGET (item->menu_item));
}

gboolean
unity_gtk_menu_item_is_separator (UnityGtkMenuItem *item)
{
  g_return_val_if_fail (UNITY_GTK_IS_MENU_ITEM (item), FALSE);

  return GTK_IS_SEPARATOR_MENU_ITEM (item->menu_item);
}

void
unity_gtk_menu_item_print (UnityGtkMenuItem *item,
                           guint             depth)
{
  gchar *indent;
  const gchar *label;

  g_return_if_fail (UNITY_GTK_IS_MENU_ITEM (item));

  indent = g_strnfill (4 * depth, ' ');
  label = unity_gtk_menu_item_get_label (item);

  g_print ("%s(UnityGtkMenuItem *) %p\n", indent, item);

  if (label != NULL)
    g_print ("%s  label = %s\n", indent, label);

  if (item->menu_item != NULL)
    g_print ("%s  menu_item = %p\n", indent, item->menu_item);

  if (item->menu_item_notify_handler_id)
    g_print ("%s  menu_item_notify_handler_id = %lu\n", indent, item->menu_item_notify_handler_id);

  if (item->parent_shell != NULL)
    g_print ("%s  parent_shell = %p\n", indent, item->parent_shell);

  if (item->child_shell != NULL)
    {
      g_print ("%s  child_shell =\n", indent);

      unity_gtk_menu_shell_print (item->child_shell, depth + 1);
    }

  g_print ("%s  item_index = %u\n", indent, item->item_index);

  g_free (indent);
}