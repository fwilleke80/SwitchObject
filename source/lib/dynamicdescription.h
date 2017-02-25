#ifndef DYNAMICDESCRIPTION_H__
#define DYNAMICDESCRIPTION_H__

#include "c4d.h"

/// Check if 'id' is a single id
Bool IsSingleID(const DescID &id, const DescID *singleid);

/// Add a new LONG CYCLE element to the description
/// @param[in] description The description that should receive the new element
/// @param[in] id The resource ID of the new element (make sure it doesn't conflict with an existing ID)
/// @param[in] groupId The resource ID of the attribute group the new element should be added to
/// @param[in] name The name of the element
/// @param[in] cycleItems BaseContainer with names of cycle items
/// @param[in] defaultValue The index of the cycle item that should be selected by default
/// @return False if an error occurred, otherwise true
Bool DescriptionAddCycle(Description *description, Int32 id, Int32 groupId, const String &name, const BaseContainer &cycleItems, Int32 defaultValue = 0);

/// Add a new LONG CYCLE element to the description
/// @param[in] description The description that should receive the new element
/// @param[in] id The resource ID of the new element (make sure it doesn't conflict with an existing ID)
/// @param[in] groupId The resource ID of the attribute group the new element should be added to
/// @param[in] name The name of the element
/// @param[in] defaultValue The default value
/// @return False if an error occurred, otherwise true
Bool DescriptionAddCheckbox(Description *description, Int32 id, Int32 groupId, const String &name, Bool defaultValue = false);

/// Add a new LONG CYCLE element to the description
/// @param[in] description The description that should receive the new element
/// @param[in] id The resource ID of the new element (make sure it doesn't conflict with an existing ID)
/// @param[in] groupId The resource ID of the attribute group the new element should be added to
/// @param[in] name The name of the element
/// @param[in] defaultValue The default string
/// @return False if an error occurred, otherwise true
Bool DescriptionAddString(Description *description, Int32 id, Int32 groupId, const String &name, const String &defaultValue = "");

/// Add a new LONG CYCLE element to the description
/// @param[in] description The description that should receive the new element
/// @param[in] id The resource ID of the new element (make sure it doesn't conflict with an existing ID)
/// @param[in] groupId The resource ID of the attribute group the new element should be added to
/// @param[in] name The name of the element
/// @param[in] acceptList BaseContainer with IDs of acceptable objects for this linkbox
/// @return False if an error occurred, otherwise true
Bool DescriptionAddLink(Description *description, Int32 id, Int32 groupId, const String &name, const BaseContainer &acceptList);

#endif // DYNAMICDESCRIPTION_H__
