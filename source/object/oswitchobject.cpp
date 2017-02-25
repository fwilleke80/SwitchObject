#include "c4d.h"
#include "ge_dynamicarray.h"
#include "dynamicdescription.h"
#include "main.h"
#include "c4d_symbols.h"
#include "oswitchobject.h"


const Int32 ID_OSWITCHOBJ = 1026830;


struct ObjectListItem
{
	String name;
	Int32 id;
	
	ObjectListItem() : id(0)
	{}
	
	ObjectListItem(BaseObject &op)
	{
		name = op.GetName();
		id = op.GetType();
	}
};


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
	
	/// Iterates the stored list with objects and puts their names or ids into a BaseContainer
	/// @param[in] names If false, the container will receive the ids of the objects. If true, the container gets the objects' names.
	/// @return The BaseContainer containing the object names
	BaseContainer ObjList2Container(Bool names);
	
	/// Iterate all children of 'parent'. The object with index 'index' will become visible, all other are hidden.
	/// @param[in] parent The parent object whose children should be iterated
	/// @param[in] index The index of the only objec tthat should be visible
	/// @return False, if an error ocurred; otherwise true
	Bool SwitchObjects(BaseObject *parent, Int32 index);
	
	/// Return the parent object of the object group we want to switch
	/// This is either the SwitchObject itself, or - in case something is linked in the group linkbox - the object linked in the group linkbox
	/// @param[in] node The SwitchObject node
	/// @return The BaseObject that's the parent of the desired object group; or nullptr if an error occurred
	BaseObject *GetGroupParent(GeListNode *node) const;
	
private:
	GeDynamicArray<ObjectListItem> m_objlist;		/// Stores a list with names of objects in the group.
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
		m_objlist.Push(ObjectListItem(*op));
		op = op->GetNext();
	}

	return count;
}


BaseContainer SwitchObject::ObjList2Container(Bool names)
{
	BaseContainer result;
	
	if (!m_objlist.Content())
	{
		result.SetString(0, GeLoadString(IDS_SWITCH_NONE));
	}
	else
	{
		for (Int32 i = 0; i < m_objlist.GetCount(); i++)
		{
			if (names)
				result.SetString(i, m_objlist[i].name);
			else
				result.SetInt32(i, m_objlist[i].id);
		}
	}

	return result;
}


Bool SwitchObject::SwitchObjects(BaseObject *parent, Int32 index)
{
	if (!m_objlist.Content() || index < 0 || index >= m_objlist.GetCount() || !parent)
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


BaseObject *SwitchObject::GetGroupParent(GeListNode *node) const
{
	if (!node)
		return nullptr;
	
	BaseObject *op = static_cast<BaseObject*>(node);

	BaseDocument *doc = node->GetDocument();
	if (!doc)
		return nullptr;
	
	BaseContainer *bc = op->GetDataInstance();
	if (!bc)
		return nullptr;

	BaseObject *result = bc->GetObjectLink(SWITCH_REMOTELINK, doc);
	if (!result)
		result = static_cast<BaseObject*>(node);

	return result;
}


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


Bool SwitchObject::GetDDescription(GeListNode *node, Description *description, DESCFLAGS_DESC &flags)
{
	if (!description->LoadDescription(node->GetType()))
		return false;

	BaseContainer acceptlink;
	acceptlink.SetInt32(Obase, 1);

	BaseContainer cycleItems;
	BaseContainer cycleIcons;
	
	if (m_objlist.Content())
	{
		cycleItems = ObjList2Container(true);
		cycleIcons = ObjList2Container(false);
	}
	else
	{
		cycleItems.SetString(0, GeLoadString(IDS_SWITCH_NONE));
	}

	// Add description elements
	DescriptionAddCycle(description, SWITCH_DROPDOWN, ID_OBJECTPROPERTIES, GeLoadString(IDS_SWITCH_DROPDOWN), cycleItems, &cycleIcons);
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
	
	GePrint(String::IntToString(type));
	
	BaseObject *op = static_cast<BaseObject*>(node);

	BaseContainer *bc = op->GetDataInstance();
	if (!bc)
		return false;

	BaseObject *parent = GetGroupParent(node);
	if (!parent)
		return false;
	
	switch (type)
	{
		case MSG_GETCUSTOMICON:
		{
			BuildObjList(parent);
			break;
		}
		case MSG_DESCRIPTION_POSTSETPARAMETER:
		{
			DescriptionPostSetValue *msgData = (DescriptionPostSetValue*)data;
			if (!data)
				return false;
			
			// Dropdown value has been changed
			if (*(msgData->descid) == SWITCH_DROPDOWN)
			{
				// TODO: Building the list should be done somewhere else
				if (m_objlist.GetCount() > 0)
				{
					// Don't do anything if the generator tick of SwitchObject is not enabled
					if (!op->GetDeformMode())
						return true;
					
					// Get index of object selected in the dropdown
					Int32 objectIndex = bc->GetInt32(SWITCH_DROPDOWN);
					
					// If 'Inherit Name' is enabled, rename the SwitchObject
					if (bc->GetBool(SWITCH_INHERITNAME))
						op->SetName(bc->GetString(SWITCH_OBJNAME, GeLoadString(IDS_OSWITCHOBJ)) + " (" + m_objlist[objectIndex].name + ")");
					
					// Switch object visibility
					SwitchObjects(parent, objectIndex);
				}
				else
				{
					// If 'Inherit Name' is enabled, rename the SwitchObject
					if (bc->GetBool(SWITCH_INHERITNAME))
						op->SetName(bc->GetString(SWITCH_OBJNAME, GeLoadString(IDS_OSWITCHOBJ)));
					
					// Select first item in dropdown
					bc->SetInt32(SWITCH_DROPDOWN, 0);
				}
			}
			break;
		}
	}
	
	return SUPER::Message(node, type, data);
}


Bool SwitchObject::GetDEnabling(GeListNode *node, const DescID &id, const GeData &t_data, DESCFLAGS_ENABLE flags, const BaseContainer *itemdesc)
{
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


Bool RegisterSwitchObject()
{
	return RegisterObjectPlugin(ID_OSWITCHOBJ, GeLoadString(IDS_OSWITCHOBJ), OBJECT_GENERATOR, SwitchObject::Alloc, "oswitchobject", AutoBitmap("oswitchobject.tif"), 0);
}
