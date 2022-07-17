// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"

/**
 * 
*/

class PMSEDITOR_API SGraphEditor_PMS : public SGraphEditor
{
public:

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual FVector2D GetPasteLocation() const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetPasteLocation();
		}
		else
		{
			return FVector2D::ZeroVector;
		}
	}

	/* Set new viewer location and optionally set the current bookmark */
	virtual void SetViewLocation(const FVector2D& Location, float ZoomAmount, const FGuid& BookmarkId = FGuid())
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->SetViewLocation(Location, ZoomAmount, BookmarkId);
		}
	}

	/**
	 * Gets the view location of the graph
	 *
	 * @param OutLocation		Will have the current view location
	 * @param OutZoomAmount		Will have the current zoom amount
	 */
	virtual void GetViewLocation(FVector2D& OutLocation, float& OutZoomAmount)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->GetViewLocation(OutLocation, OutZoomAmount);
		}
	}

	/**
	 * Gets the current graph view bookmark
	 *
	 * @param OutBookmarkId		Will have the current bookmark ID
	 */
	virtual void GetViewBookmark(FGuid& OutBookmarkId)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->GetViewBookmark(OutBookmarkId);
		}
	}

	/** Check if node title is visible with optional flag to ensure it is */
	virtual bool IsNodeTitleVisible(const class UEdGraphNode* Node, bool bRequestRename)
	{
		bool bResult = false;
		if (Implementation_PMS.IsValid())
		{
			bResult = Implementation_PMS->IsNodeTitleVisible(Node, bRequestRename);
		}
		return bResult;
	}

	/* Lock two graph editors together */
	virtual void LockToGraphEditor(TWeakPtr<SGraphEditor> Other)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->LockToGraphEditor(Other);
		}
	}

	/* Unlock two graph editors from each other */
	virtual void UnlockFromGraphEditor(TWeakPtr<SGraphEditor> Other)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->UnlockFromGraphEditor(Other);
		}
	}

	/** Bring the specified node into view */
	virtual void JumpToNode( const class UEdGraphNode* JumpToMe, bool bRequestRename = false, bool bSelectNode = true )
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->JumpToNode(JumpToMe, bRequestRename, bSelectNode);
		}
	}

	/** Bring the specified pin into view */
	virtual void JumpToPin( const class UEdGraphPin* JumpToMe )
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->JumpToPin(JumpToMe);
		}
	}
	

	/*Set the pin visibility mode*/
	virtual void SetPinVisibility(EPinVisibility InVisibility)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->SetPinVisibility(InVisibility);
		}
	}

	/** Register an active timer on the graph editor. */
	virtual TSharedRef<FActiveTimerHandle> RegisterActiveTimer(float TickPeriod, FWidgetActiveTimerDelegate TickFunction)
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->RegisterActiveTimer(TickPeriod, TickFunction);
		}
		return TSharedPtr<FActiveTimerHandle>().ToSharedRef();
	}

	/** @return a reference to the list of selected graph nodes */
	virtual const FGraphPanelSelectionSet& GetSelectedNodes() const
	{
		static FGraphPanelSelectionSet NoSelection;

		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetSelectedNodes();
		}
		else
		{
			return NoSelection;
		}
	}

	/** Clear the selection */
	virtual void ClearSelectionSet()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->ClearSelectionSet();
		}
	}

	/** Set the selection status of a node */
	virtual void SetNodeSelection(UEdGraphNode* Node, bool bSelect)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->SetNodeSelection(Node, bSelect);
		}
	}
	
	/** Select all nodes */
	virtual void SelectAllNodes()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->SelectAllNodes();
		}		
	}

	virtual class UEdGraphPin* GetGraphPinForMenu()
	{
		if ( Implementation_PMS.IsValid() )
		{
			return Implementation_PMS->GetGraphPinForMenu();
		}
		else
		{
			return NULL;
		}
	}

	virtual class UEdGraphNode* GetGraphNodeForMenu()
	{
		if ( Implementation_PMS.IsValid() )
		{
			return Implementation_PMS->GetGraphNodeForMenu();
		}
		else
		{
			return NULL;
		}
	}

	// Zooms out to fit either all nodes or only the selected ones
	virtual void ZoomToFit(bool bOnlySelection)
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->ZoomToFit(bOnlySelection);
		}
	}

	/** Get Bounds for selected nodes, false if nothing selected*/
	virtual bool GetBoundsForSelectedNodes( class FSlateRect& Rect, float Padding  )
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetBoundsForSelectedNodes(Rect, Padding);
		}
		return false;
	}

	/** Get Bounds for the specified node, returns false on failure */
	virtual bool GetBoundsForNode( const UEdGraphNode* InNode, class FSlateRect& Rect, float Padding ) const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetBoundsForNode(InNode, Rect, Padding);
		}
		return false;
	}

	virtual void StraightenConnections()
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->StraightenConnections();
		}
	}

	virtual void StraightenConnections(UEdGraphPin* SourcePin, UEdGraphPin* PinToAlign = nullptr) const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->StraightenConnections(SourcePin, PinToAlign);
		}
	}

	virtual void RefreshNode(UEdGraphNode& Node)
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->RefreshNode(Node);
		}
	}
	
	/** Invoked when the Graph being edited changes in some way. */
	virtual void NotifyGraphChanged()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->NotifyGraphChanged();
		}
	}

	/* Get the title bar if there is one */
	virtual TSharedPtr<SWidget> GetTitleBar() const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetTitleBar();
		}
		return TSharedPtr<SWidget>();
	}

	/** Show notification on graph */
	virtual void AddNotification(FNotificationInfo& Info, bool bSuccess)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->AddNotification(Info, bSuccess);
		}
	}

	/** Capture keyboard */
	virtual void CaptureKeyboard()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->CaptureKeyboard();
		}
	}

	/** Sets the current node, pin and connection factory. */
	virtual void SetNodeFactory(const TSharedRef<class FGraphNodeFactory>& NewNodeFactory)
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->SetNodeFactory(NewNodeFactory);
		}
	}
	
	virtual void OnCollapseNodes()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnCollapseNodes();
		}
	}

	virtual bool CanCollapseNodes() const
	{
		return Implementation_PMS.IsValid() ? Implementation_PMS->CanCollapseNodes() : false;
	}

	virtual void OnExpandNodes()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnExpandNodes();
		}
	}

	virtual bool CanExpandNodes() const
	{
		return Implementation_PMS.IsValid() ? Implementation_PMS->CanExpandNodes() : false;
	}

	virtual void OnAlignTop()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnAlignTop();
		}
	}

	virtual void OnAlignMiddle()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnAlignMiddle();
		}
	}

	virtual void OnAlignBottom()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnAlignBottom();
		}
	}

	virtual void OnAlignLeft()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnAlignLeft();
		}
	}

	virtual void OnAlignCenter()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnAlignCenter();
		}
	}

	virtual void OnAlignRight()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnAlignRight();
		}
	}


	virtual void OnStraightenConnections()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnStraightenConnections();
		}
	}


	virtual void OnDistributeNodesH()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnDistributeNodesH();
		}
	}

	virtual void OnDistributeNodesV()
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnDistributeNodesV();
		}
	}


	virtual int32 GetNumberOfSelectedNodes() const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetNumberOfSelectedNodes();
		}
		return 0;
	}


	/** Returns the currently selected node if there is a single node selected (if there are multiple nodes selected or none selected, it will return nullptr) */
	virtual UEdGraphNode* GetSingleSelectedNode() const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetSingleSelectedNode();
		}
		return nullptr;
	}
	

	/** Returns the graph panel used for this graph editor */
	virtual SGraphPanel* GetGraphPanel() const
	{
		if (Implementation_PMS.IsValid())
		{
			return Implementation_PMS->GetGraphPanel();
		}
		return nullptr;
	}

	/** Invoked when the underlying Graph is being changed. */
	virtual void OnGraphChanged(const struct FEdGraphEditAction& InAction) override
	{
		if (Implementation_PMS.IsValid())
		{
			Implementation_PMS->OnGraphChanged(InAction);
		}
	}

private:
	static void RegisterGraphEditor_PMS(const TSharedRef<SGraphEditor_PMS>& InGraphEditor);

	void ConstructImplementation_PMS( const FArguments& InArgs );

	TSharedPtr<SGraphEditor_PMS> Implementation_PMS;

	static TArray< TWeakPtr<SGraphEditor_PMS> > AllInstances_PMS;
	
	friend class FPMSEditorModule;
};
