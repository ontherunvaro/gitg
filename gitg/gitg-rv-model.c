#include "gitg-rv-model.h"
#include <time.h>

#define GITG_RV_MODEL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GITG_TYPE_RV_MODEL, GitgRvModelPrivate))

static void gitg_rv_model_tree_model_iface_init(GtkTreeModelIface *iface);

G_DEFINE_TYPE_EXTENDED (GitgRvModel, gitg_rv_model, GTK_TYPE_LIST_STORE, 0,
	G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, gitg_rv_model_tree_model_iface_init));

enum
{
	OBJECT_COLUMN,
	SUBJECT_COLUMN,
	AUTHOR_COLUMN,
	DATE_COLUMN,
	N_COLUMNS
};

static GtkTreeModelIface parent_iface = { 0, };

static int
gitg_rv_model_get_n_columns (GtkTreeModel *self)
{
	/* validate our parameters */
	g_return_val_if_fail(GITG_RV_MODEL(self), 0);

	return N_COLUMNS;
}

static GType
gitg_rv_model_get_column_type(GtkTreeModel *self, int column)
{
	GType types[] = {
		GITG_TYPE_REVISION,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING
	};

	/* validate our parameters */
	g_return_val_if_fail(GITG_IS_RV_MODEL(self), G_TYPE_INVALID);
	g_return_val_if_fail(column >= 0 && column < N_COLUMNS, G_TYPE_INVALID);

	return types[column];
}

/* retreive an object from our parent's data storage,
 * unref the returned object when done */
static GitgRevision *
gitg_rv_model_get_object(GitgRvModel *self, GtkTreeIter *iter)
{
	GValue value = { 0, };
	GitgRevision *obj;

	/* validate our parameters */
	g_return_val_if_fail(GITG_IS_RV_MODEL(self), NULL);
	g_return_val_if_fail(iter != NULL, NULL);

	/* retreive the object using our parent's interface, take our own
	 * reference to it */
	parent_iface.get_value(GTK_TREE_MODEL(self), iter, 0, &value);
	obj = GITG_REVISION(g_value_dup_object(&value));

	g_value_unset (&value);

	return obj;
}

static gchar *
timestamp_to_str(guint64 timestamp)
{
	struct tm *tms = localtime((time_t *)&timestamp);
	char buf[255];
	
	strftime(buf, 255, "%Y-%m-%d %H:%M:%S", tms);
	return g_strdup(buf);
}

static void
gitg_rv_model_get_value(GtkTreeModel *self, GtkTreeIter *iter, int column,
		GValue *value)
{
	GitgRevision *obj;

	/* validate our parameters */
	g_return_if_fail(GITG_IS_RV_MODEL(self));
	g_return_if_fail(iter != NULL);
	g_return_if_fail(column >= 0 && column < N_COLUMNS);
	g_return_if_fail(value != NULL);

	/* get the object from our parent's storage */
	obj = gitg_rv_model_get_object(GITG_RV_MODEL(self), iter);

	/* initialise our GValue to the required type */
	g_value_init(value, gitg_rv_model_get_column_type(GTK_TREE_MODEL(self), column));

	switch (column)
	{
		case OBJECT_COLUMN:
			/* the object itself was requested */
			g_value_set_object(value, obj);
			break;
		case AUTHOR_COLUMN:
			g_value_set_string(value, gitg_revision_get_author(obj));
			break;
		case SUBJECT_COLUMN:
			g_value_set_string(value, gitg_revision_get_subject(obj));
			break;
		case DATE_COLUMN:
			g_value_take_string(value, timestamp_to_str(gitg_revision_get_timestamp(obj)));
			break;
		default:
			g_assert_not_reached();
	}

	/* release the reference gained from gitg_rv_model	_get_object() */
	g_object_unref(obj);
}

static void
gitg_rv_model_tree_model_iface_init(GtkTreeModelIface *iface)
{
	parent_iface = *iface;
	
	iface->get_n_columns = gitg_rv_model_get_n_columns;
	iface->get_column_type = gitg_rv_model_get_column_type;
	iface->get_value = gitg_rv_model_get_value;
}

static void
gitg_rv_model_finalize(GObject *object)
{
	G_OBJECT_CLASS (gitg_rv_model_parent_class)->finalize(object);
}

static void 
gitg_rv_model_class_init(GitgRvModelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = gitg_rv_model_finalize;
}

static void
gitg_rv_model_init(GitgRvModel *object)
{
	//self->priv = GITG_RV_MODEL_GET_PRIVATE(self);
	GType types[] = { GITG_TYPE_REVISION };

	gtk_list_store_set_column_types(GTK_LIST_STORE(object), 1, types);
}

void
gitg_rv_model_add(GitgRvModel *self, GitgRevision *obj, GtkTreeIter *iter)
{
	GtkTreeIter iter1;

	/* validate our parameters */
	g_return_if_fail(GITG_IS_RV_MODEL(self));
	g_return_if_fail(GITG_IS_REVISION(obj));

	/* put this object in our data storage */
	gtk_list_store_append(GTK_LIST_STORE(self), &iter1);
	gtk_list_store_set(GTK_LIST_STORE(self), &iter1, 0, obj, -1);

	/* here you would connect up signals (e.g. ::notify) to notice when your
	 * object has changed */

	/* return the iter if the user cares */
	if (iter)
		*iter = iter1;
}

GitgRvModel *
gitg_rv_model_new()
{
	return g_object_new(GITG_TYPE_RV_MODEL, NULL);
}

gboolean
gitg_rv_model_find_by_hash(GitgRvModel *store, gchar const *hash, GtkTreeIter *iter)
{
	g_return_val_if_fail(GITG_IS_RV_MODEL(store), FALSE);
	
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), iter))
	{
		do
		{
			GitgRevision *rv;
			gtk_tree_model_get(GTK_TREE_MODEL(store), iter, 0, &rv, -1);
			gchar const *phash = gitg_revision_get_hash(rv);
			g_object_unref(rv);

			if (strncmp(hash, phash, 20) == 0)
				return TRUE;
		} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(store), iter));
	}

	return FALSE;
}
