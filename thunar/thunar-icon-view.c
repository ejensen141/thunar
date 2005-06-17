/* $Id$ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <thunar/thunar-icon-view.h>



static void             thunar_icon_view_class_init         (ThunarIconViewClass *klass);
static void             thunar_icon_view_view_init          (ThunarViewIface     *iface);
static void             thunar_icon_view_init               (ThunarIconView      *icon_view);
static void             thunar_icon_view_item_activated     (ExoIconView         *view,
                                                             GtkTreePath         *path);
static void             thunar_icon_view_selection_changed  (ExoIconView         *view);
static GList           *thunar_icon_view_get_selected_files (ThunarView          *view);



struct _ThunarIconViewClass
{
  ExoIconViewClass __parent__;
};

struct _ThunarIconView
{
  ExoIconView __parent__;
};



G_DEFINE_TYPE_WITH_CODE (ThunarIconView,
                         thunar_icon_view,
                         EXO_TYPE_ICON_VIEW,
                         G_IMPLEMENT_INTERFACE (THUNAR_TYPE_VIEW,
                                                thunar_icon_view_view_init));



static void
thunar_icon_view_class_init (ThunarIconViewClass *klass)
{
  ExoIconViewClass *exoicon_view_class;

  exoicon_view_class = EXO_ICON_VIEW_CLASS (klass);
  exoicon_view_class->item_activated = thunar_icon_view_item_activated;
  exoicon_view_class->selection_changed = thunar_icon_view_selection_changed;
}



static void
thunar_icon_view_view_init (ThunarViewIface *iface)
{
  iface->get_model = (gpointer) exo_icon_view_get_model;
  iface->set_model = (gpointer) exo_icon_view_set_model;
  iface->get_selected_files = thunar_icon_view_get_selected_files;
}



static void
thunar_icon_view_init (ThunarIconView *icon_view)
{
  /* initialize the icon view properties */
  exo_icon_view_set_text_column (EXO_ICON_VIEW (icon_view), THUNAR_LIST_MODEL_COLUMN_NAME);
  exo_icon_view_set_pixbuf_column (EXO_ICON_VIEW (icon_view), THUNAR_LIST_MODEL_COLUMN_ICON_NORMAL);
  exo_icon_view_set_selection_mode (EXO_ICON_VIEW (icon_view), GTK_SELECTION_MULTIPLE);
}



static void
thunar_icon_view_item_activated (ExoIconView *view,
                                 GtkTreePath *path)
{
  ThunarIconView *icon_view = THUNAR_ICON_VIEW (view);
  GtkTreeModel   *model;
  GtkTreeIter     iter;
  ThunarFile     *file;

  g_return_if_fail (THUNAR_IS_ICON_VIEW (icon_view));

  /* tell the controlling component, that the user activated a file */
  model = exo_icon_view_get_model (view);
  gtk_tree_model_get_iter (model, &iter, path);
  file = thunar_list_model_get_file (THUNAR_LIST_MODEL (model), &iter);
  thunar_view_file_activated (THUNAR_VIEW (icon_view), file);

  /* invoke the item_activated method on the parent class */
  if (EXO_ICON_VIEW_CLASS (thunar_icon_view_parent_class)->item_activated != NULL)
    EXO_ICON_VIEW_CLASS (thunar_icon_view_parent_class)->item_activated (view, path);
}



static void
thunar_icon_view_selection_changed (ExoIconView *view)
{
  g_return_if_fail (THUNAR_IS_ICON_VIEW (view));

  /* tell everybody that we have a new selection of files */
  thunar_view_file_selection_changed (THUNAR_VIEW (view));

  if (EXO_ICON_VIEW_CLASS (thunar_icon_view_parent_class)->selection_changed != NULL)
    EXO_ICON_VIEW_CLASS (thunar_icon_view_parent_class)->selection_changed (view);
}



static GList*
thunar_icon_view_get_selected_files (ThunarView *view)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;
  ThunarFile   *file;
  GList        *files = NULL;
  GList        *paths;
  GList        *lp;

  g_return_val_if_fail (THUNAR_IS_ICON_VIEW (view), NULL);

  paths = exo_icon_view_get_selected_items (EXO_ICON_VIEW (view));
  if (paths != NULL)
    {
      model = exo_icon_view_get_model (EXO_ICON_VIEW (view));
      for (lp = g_list_last (paths); lp != NULL; lp = lp->prev)
        {
          gtk_tree_model_get_iter (model, &iter, lp->data);
          file = thunar_list_model_get_file (THUNAR_LIST_MODEL (model), &iter);
          files = g_list_prepend (files, file);
          gtk_tree_path_free (lp->data);
        }
      g_list_free (paths);
    }

  return files;
}



/**
 * thunar_icon_view_new:
 *
 * Allocates a new #ThunarIconView instance, which is not
 * associated with any #ThunarListModel yet.
 *
 * Return value: the newly allocated #ThunarIconView instance.
 **/
GtkWidget*
thunar_icon_view_new (void)
{
  return g_object_new (THUNAR_TYPE_ICON_VIEW, NULL);
}


