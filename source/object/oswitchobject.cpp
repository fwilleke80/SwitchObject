#include "c4d.h"
#include "ge_dynamicarray.h"
#include "dynamicdescription.h"
#include "main.h"
#include "c4d_symbols.h"
#include "oswitchobject.h"


const Int32 ID_OSWITCHOBJ = 1026830;


class SwitchObject : public ObjectData
{
	INSTANCEOF(SwitchObject, ObjectData);

public:
	virtual Bool Init(GeListNode *node);
	virtual Bool GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags);
	virtual Bool GetDEnabling(GeListNode *node, const DescID &id, const GeData &t_data, DESCFLAGS_ENABLE flags, const BaseContainer *itemdesc);
	virtual Bool Message(GeListNode *node, Int32 type, void *data);
	
	static NodeData *Alloc()
	{
		return NewObjClear(SwitchObject);
	}
	
private:
	/// Iterate all children of 'parent' and store their names in m_objlist
	/// @param[in] parent The parent object whose children should be iterated
	/// @return The number of objects in the list, or NOTOK if an error occurred
	Int32 BuildObjList(BaseObject *parent);
	
	/// Iterates the stored list with object names and puts them into a BaseContainer
	/// @return The BaseContainer containing the object names
	BaseContainer ObjList2Container();
	
	/// Iterate all children of 'parent'. The object with index 'index' will become visible, all other are hidden.
	/// @param[in] parent The parent object whose children should be iterated
	/// @param[in] index The index of the only objec tthat should be visible
	/// @return False, if an error ocurred; otherwise true
	Bool SwitchObjects(BaseObject *parent, Int32 index);
	
	/// Return the parent object of the object group we want to switch
	/// This is either the SwitchObject itself, or - in case something is linked in the group linkbox - the object linked in the group linkbox
	/// @param[in] node The SwitchObject node
	/// @return The BaseObject that's the parent of the desired object group; or nullptr if an error occurred
	BaseObject *GetGroupParent(GeListNode *node);
	
private:
	GeDynamicArray<String> m_objlist;		/// Stores a list with names of objects in the group.
};


Int32 SwitchObject::BuildObjList(BaseObject *parent)
{
	if (!parent)
		return NOTOK;

	m_objlist.FreeArray();
	Int32 count = 0;
	BaseObject *op = parent->GetDown();
	while (op)
	{
		count++;
		m_objlist.Push(op->GetName());
		op = op->GetNext();
	}

	return count;
}


BaseContainer SwitchObject::ObjList2Container()
{
	BaseContainer result;
	
	if (!m_objlist.Content())
	{
		result.SetString(0, GeLoadString(IDS_SWITCH_NONE));
	}
	else
	{
		for (Int32 i = 0; i < m_objlist.GetCount(); i++)
			result.SetString(i, m_objlist[i]);
	}

	return result;
}


Bool SwitchObject::SwitchObjects(BaseObject *parent, Int32 index)
{
	if (!m_objlist.Content() || index < 0 || index >= m_objlist.GetCount() || !parent || !parent->GetDown())
		return false;

	Int32 i = 0;
	BaseObject *op = parent->GetDown();

	while (op)
	{
		if (i == index)
		{
			op->SetEditorMode(MODE_UNDEF);
			op->SetRenderMode(MODE_UNDEF);
		}
		else
		{
			op->SetEditorMode(MODE_OFF);
			op->SetRenderMode(MODE_OFF);
		}
		op = op->GetNext();
		i++;
	}

	return true;
}


BaseObject *SwitchObject::GetGroupParent(GeListNode *node)
{
	BaseDocument *doc = node->GetDocument();
	if (!doc)
		return nullptr;

	BaseContainer *bc = static_cast<BaseObject*>(node)->GetDataInstance();
	if (!bc)
		return nullptr;

	BaseObject *result = bc->GetObjectLink(SWITCH_REMOTELINK, doc);
	if (!result)
		result = static_cast<BaseObject*>(node);

	return result;
}


// Initialize settings
Bool SwitchObject::Init(GeListNode *node)
{
	BaseObject *op = static_cast<BaseObject*>(node);
	if (!op)
		return false;

	BaseContainer *data = op->GetDataInstance();
	if (!data)
		return false;

	data->SetInt32(SWITCH_DROPDOWN, 0);
	data->SetString(SWITCH_OBJNAME, GeLoadString(IDS_OSWITCHOBJ));

	return SUPER::Init(node);
}


// Build the dynamic descriptions (it's only a long cycle)
Bool SwitchObject::GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags)
{
	// SWITCH_DROPDOWN
	if (!description->LoadDescription(node->GetType()))
		return false;

	BaseContainer acceptlink;
	acceptlink.SetInt32(Obase, 1);

	BaseContainer cycleItems;
	
	if (m_objlist.Content())
		cycleItems = ObjList2Container();
	else
		cycleItems.SetString(0, GeLoadString(IDS_SWITCH_NONE));

	// Add description elements
	DescriptionAddCycle(description, SWITCH_DROPDOWN, ID_OBJECTPROPERTIES, GeLoadString(IDS_SWITCH_DROPDOWN), cycleItems);
	DescriptionAddCheckbox(description, SWITCH_INHERITNAME, ID_OBJECTPROPERTIES, GeLoadString(IDS_SWITCH_INHERITNAME), true);
	DescriptionAddString(description, SWITCH_OBJNAME, ID_OBJECTPROPERTIES, GeLoadString(IDS_SWITCH_OBJNAME), GeLoadString(IDS_OSWITCHOBJ));
	DescriptionAddLink(description, SWITCH_REMOTELINK, ID_OBJECTPROPERTIES, GeLoadString(IDS_SWITCH_REMOTELINK), acceptlink);

	flags |= DESCFLAGS_DESC_LOADED;

	return SUPER::GetDDescription(node,description,flags);
}


Bool SwitchObject::Message(GeListNode *node, Int32 type, void *data)
{
	if (!node)
		return false;
	
	BaseObject *op = static_cast<BaseObject*>(node);

	BaseContainer *bc = op->GetDataInstance();
	if (!bc)
		return false;

	BaseObject *parent = GetGroupParent(node);
	if (!parent)
		return false;
	
	if (BuildObjList(parent) > 0)
	{
		if (!((BaseObject*)node)->GetDeformMode()) return true;

		Int32 index = bc->GetInt32(SWITCH_DROPDOWN);

		if (bc->GetBool(SWITCH_INHERITNAME))
		{
			String name = bc->GetString(SWITCH_OBJNAME, GeLoadString(IDS_OSWITCHOBJ));
			((BaseObject*)node)->SetName(name + " (" + m_objlist[index] + ")");
		}

		SwitchObjects(parent, index);
	}
	else
	{
		if (bc->GetBool(SWITCH_INHERITNAME))
		{
			String name = bc->GetString(SWITCH_OBJNAME, GeLoadString(IDS_OSWITCHOBJ));
			((BaseObject*)node)->SetName(name);
		}

		bc->SetInt32(SWITCH_DROPDOWN, 0);
	}

	return SUPER::Message(node, type, data);
}


// Disable parameters
Bool SwitchObject::GetDEnabling(GeListNode *node, const DescID &id, const GeData &t_data, DESCFLAGS_ENABLE flags, const BaseContainer *itemdesc)
{
	// Enable and disable ('gray out') user controls
	BaseContainer *data = static_cast<BaseObject*>(node)->GetDataInstance();
	if (!data)
		return false;
	
	BaseDocument *doc = node->GetDocument();
	if (!doc)
		return false;

	switch (id[0].id)
	{
		case SWITCH_DROPDOWN:
			return (m_objlist.Content()) ? true : false;
		case SWITCH_OBJNAME:
			return data->GetBool(SWITCH_INHERITNAME) && !data->GetLink(SWITCH_REMOTELINK, doc);
	}

	return SUPER::GetDEnabling(node, id, t_data, flags, itemdesc);
}


////////////////////////////////////////////////////////////////////
// Register function
////////////////////////////////////////////////////////////////////
Bool RegisterSwitchObject()
{
	return RegisterObjectPlugin(ID_OSWITCHOBJ, GeLoadString(IDS_OSWITCHOBJ), OBJECT_GENERATOR, SwitchObject::Alloc, "oswitchobject", AutoBitmap("oswitchobject.tif"), 0);
}
