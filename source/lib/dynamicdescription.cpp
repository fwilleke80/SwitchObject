#include "dynamicdescription.h"


Bool IsSingleID(const DescID &id, const DescID *singleid)
{
	return !singleid || id.IsPartOf(*singleid, NULL);
}


Bool DescriptionAddCycle(Description *description, Int32 id, Int32 groupId, const String &name, const BaseContainer &cycleItems, Int32 defaultValue)
{
	if (!description)
		return false;
	
	const DescID *singleId = description->GetSingleDescID();
	
	if (!singleId || IsSingleID(id, singleId))
	{
		BaseContainer params = GetCustomDataTypeDefault(DTYPE_LONG);
		params.SetString(DESC_NAME, name);
		params.SetInt32(DESC_DEFAULT, defaultValue);
		params.SetInt32(DESC_ANIMATE, DESC_ANIMATE_ON);
		params.SetInt32(DESC_CUSTOMGUI, CUSTOMGUI_CYCLE);
		params.SetContainer(DESC_CYCLE, cycleItems);
		
		return description->SetParameter(DescLevel(id, DTYPE_LONG, 0), params, groupId);
	}
	return true;
}


Bool DescriptionAddCheckbox(Description *description, Int32 id, Int32 groupId, const String &name, Bool defaultValue)
{
	if (!description)
		return false;
	
	const DescID *singleId = description->GetSingleDescID();
	
	if (!singleId || IsSingleID(id, singleId))
	{
		BaseContainer params = GetCustomDataTypeDefault(DTYPE_BOOL);
		params.SetString(DESC_NAME, name);
		params.SetBool(DESC_DEFAULT, defaultValue);
		params.SetInt32(DESC_ANIMATE, DESC_ANIMATE_ON);
		
		return description->SetParameter(DescLevel(id, DTYPE_BOOL, 0), params, groupId);
	}
	
	return true;
}


Bool DescriptionAddString(Description *description, Int32 id, Int32 groupId, const String &name, const String &defaultValue)
{
	if (!description)
		return false;
	
	const DescID *singleId = description->GetSingleDescID();
	
	if (!singleId || IsSingleID(id, singleId))
	{
		BaseContainer params = GetCustomDataTypeDefault(DTYPE_STRING);
		params.SetString(DESC_NAME, name);
		params.SetString(DESC_DEFAULT, defaultValue);
		params.SetInt32(DESC_ANIMATE, DESC_ANIMATE_ON);
		
		return description->SetParameter(DescLevel(id, DTYPE_STRING, 0), params, groupId);
	}
	
	return true;
}


Bool DescriptionAddLink(Description *description, Int32 id, Int32 groupId, const String &name, const BaseContainer &acceptList)
{
	if (!description)
		return false;
	
	const DescID *singleId = description->GetSingleDescID();
	
	if (!singleId || IsSingleID(id, singleId))
	{
		BaseContainer params = GetCustomDataTypeDefault(DTYPE_BASELISTLINK);
		params.SetString(DESC_NAME, name);
		params.SetInt32(DESC_ANIMATE, DESC_ANIMATE_ON);
		params.SetContainer(DESC_ACCEPT, acceptList);
		
		return description->SetParameter(DescLevel(id, DTYPE_BASELISTLINK, 0), params, groupId);
	}
	
	return true;
}
