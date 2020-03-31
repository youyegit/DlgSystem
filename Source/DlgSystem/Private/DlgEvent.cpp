// Copyright Csaba Molnar, Daniel Butum. All Rights Reserved.
#include "DlgEvent.h"

#include "DlgSystemPrivatePCH.h"
#include "DlgReflectionHelper.h"
#include "DlgDialogueParticipant.h"
#include "Logging/DlgLogger.h"

void FDlgEvent::Call(UObject* TargetParticipant) const
{
	if (!ValidateIsParticipantValid(TargetParticipant))
	{
		return;
	}

	switch (EventType)
	{
	case EDlgEventType::Event:
		IDlgDialogueParticipant::Execute_OnDialogueEvent(TargetParticipant, EventName);
		break;

	case EDlgEventType::ModifyInt:
		IDlgDialogueParticipant::Execute_ModifyIntValue(TargetParticipant, EventName, bDelta, IntValue);
		break;
	case EDlgEventType::ModifyFloat:
		IDlgDialogueParticipant::Execute_ModifyFloatValue(TargetParticipant, EventName, bDelta, FloatValue);
		break;
	case EDlgEventType::ModifyBool:
		IDlgDialogueParticipant::Execute_ModifyBoolValue(TargetParticipant, EventName, bValue);
		break;
	case EDlgEventType::ModifyName:
		IDlgDialogueParticipant::Execute_ModifyNameValue(TargetParticipant, EventName, NameValue);
		break;

	case EDlgEventType::ModifyClassIntVariable:
		UDlgReflectionHelper::ModifyVariable<UIntProperty>(TargetParticipant, EventName, IntValue, bDelta);
		break;
	case EDlgEventType::ModifyClassFloatVariable:
		UDlgReflectionHelper::ModifyVariable<UFloatProperty>(TargetParticipant, EventName, FloatValue, bDelta);
		break;
	case EDlgEventType::ModifyClassBoolVariable:
		UDlgReflectionHelper::SetVariable<UBoolProperty>(TargetParticipant, EventName, bValue);
		break;
	case EDlgEventType::ModifyClassNameVariable:
		UDlgReflectionHelper::SetVariable<UNameProperty>(TargetParticipant, EventName, NameValue);
		break;

	default:
		checkNoEntry();
	}
}

bool FDlgEvent::ValidateIsParticipantValid(const UObject* Participant) const
{
	if (IsValid(Participant))
	{
		return true;
	}

	FDlgLogger::Get().Errorf(
		TEXT("Event failed: invalid participant! ParticipantName = %s, EventName = %s"),
		*ParticipantName.ToString(), *EventName.ToString()
	);
	return false;
}

FArchive& operator<<(FArchive& Ar, FDlgEvent& DlgEvent)
{
	Ar << DlgEvent.ParticipantName;
	Ar << DlgEvent.EventName;
	Ar << DlgEvent.IntValue;
	Ar << DlgEvent.FloatValue;
	Ar << DlgEvent.NameValue;
	Ar << DlgEvent.bDelta;
	Ar << DlgEvent.bValue;
	Ar << DlgEvent.EventType;
	return Ar;
}

bool FDlgEvent::operator==(const FDlgEvent& Other) const
{
	return ParticipantName == Other.ParticipantName &&
		   EventName == Other.EventName &&
		   IntValue == Other.IntValue &&
		   FMath::IsNearlyEqual(FloatValue, Other.FloatValue, KINDA_SMALL_NUMBER) &&
		   bDelta == Other.bDelta &&
		   bValue == Other.bValue &&
		   EventType == Other.EventType;
}

void FDlgCustomEvent::Call(UObject* Participant) const
{
	if (!IsValid())
	{
		FDlgLogger::Get().Warning(TEXT("Custom Event is empty (not valid)"));
		return;
	}

	Event->EnterEvent(Participant);
}


FArchive& operator<<(FArchive &Ar, FDlgCustomEvent& DlgEvent)
{
	Ar << DlgEvent.ParticipantName;
	Ar << DlgEvent.Event;

	return Ar;
}

bool FDlgCustomEvent::operator==(const FDlgCustomEvent& Other) const
{
	return ParticipantName == Other.ParticipantName &&
		   Event == Other.Event;
}
