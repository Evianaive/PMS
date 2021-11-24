// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/SNodeFlagCheckBox.h"
#include "Widgets/SBoxPanel.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"

FReply SNodeFlagCheckBox::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	const EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);
	const bool bMustBePressed = InputClickMethod == EButtonClickMethod::DownAndUp || InputClickMethod == EButtonClickMethod::PreciseClick;
	const bool bMeetsPressedRequirements = (!bMustBePressed || (bIsPressed && bMustBePressed));

	if (bMeetsPressedRequirements && ((MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton || MouseEvent.IsTouchEvent())))
	{
		bIsPressed = false;
	
		if(InputClickMethod == EButtonClickMethod::MouseDown )
		{
			// NOTE: If we're configured to click on mouse-down/precise-tap, then we never capture the mouse thus
			//       may never receive an OnMouseButtonUp() call.  We make sure that our bIsPressed
			//       state is reset by overriding OnMouseLeave().
		}
		else
		{
			const bool IsUnderMouse = MyGeometry.IsUnderLocation( MouseEvent.GetScreenSpacePosition() );
			if ( IsUnderMouse )
			{
				// If we were asked to allow the button to be clicked on mouse up, regardless of whether the user
				// pressed the button down first, then we'll allow the click to proceed without an active capture
				if(InputClickMethod == EButtonClickMethod::MouseUp || HasMouseCapture() )
				{
					ToggleCheckedState();
					const ECheckBoxState State = IsCheckboxChecked.Get();
					if(State == ECheckBoxState::Checked)
					{
						PlayCheckedSound();
					}
					else if(State == ECheckBoxState::Unchecked)
					{
						PlayUncheckedSound();
					}
				}
			}
		}
	
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}