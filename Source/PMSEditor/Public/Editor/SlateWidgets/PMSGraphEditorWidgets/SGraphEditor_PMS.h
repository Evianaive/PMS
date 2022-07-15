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
		if (Implementation.IsValid())
		{
			return Implementation->GetPasteLocation();
		}
		else
		{
			return FVector2D::ZeroVector;
		}
	}

	/* Set new viewer location and optionally set the current bookmark */
	virtual void SetViewLocation(const FVector2D& Location, float ZoomAmount, const FGuid& BookmarkId = FGuid())
	{
		if (Implementation.IsValid())
		{
			Implementation->SetViewLocation(Location, ZoomAmount, BookmarkId);
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
		if (Implementation.IsValid())
		{
			Implementation->GetViewLocation(OutLocation, OutZoomAmount);
		}
	}

	/**
	 * Gets the current graph view bookmark
	 *
	 * @param OutBookmarkId		Will have the current bookmark ID
	 */
	virtual void GetViewBookmark(FGuid& OutBookmarkId)
	{
		if (Implementation.IsValid())
		{
			Implementation->GetViewBookmark(OutBookmarkId);
		}
	}

	/** Check if node title is visible with optional flag to ensure it is */
	virtual bool IsNodeTitleVisible(const class UEdGraphNode* Node, bool bRequestRename)
	{
		bool bResult = false;
		if (Implementation.IsValid())
		{
			bResult = Implementation->IsNodeTitleVisible(Node, bRequestRename);
		}
		return bResult;
	}

	/* Lock two graph editors together */
	virtual void LockToGraphEditor(TWeakPtr<SGraphEditor> Other)
	{
		if (Implementation.IsValid())
		{
			Implementation->LockToGraphEditor(Other);
		}
	}

	/* Unlock two graph editors from each other */
	virtual void UnlockFromGraphEditor(TWeakPtr<SGraphEditor> Other)
	{
		if (Implementation.IsValid())
		{
			Implementation->UnlockFromGraphEditor(Other);
		}
	}

	/** Bring the specified node into view */
	virtual void JumpToNode( const class UEdGraphNode* JumpToMe, bool bRequestRename = false, bool bSelectNode = true )
	{
		if (Implementation.IsValid())
		{
			Implementation->JumpToNode(JumpToMe, bRequestRename, bSelectNode);
		}
	}

	/** Bring the specified pin into view */
	virtual void JumpToPin( const class UEdGraphPin* JumpToMe )
	{
		if (Implementation.IsValid())
		{
			Implementation->JumpToPin(JumpToMe);
		}
	}
	

	/*Set the pin visibility mode*/
	virtual void SetPinVisibility(EPinVisibility InVisibility)
	{
		if (Implementation.IsValid())
		{
			Implementation->SetPinVisibility(InVisibility);
		}
	}

	/** Register an active timer on the graph editor. */
	virtual TSharedRef<FActiveTimerHandle> RegisterActiveTimer(float TickPeriod, FWidgetActiveTimerDelegate TickFunction)
	{
		if (Implementation.IsValid())
		{
			return Implementation->RegisterActiveTimer(TickPeriod, TickFunction);
		}
		return TSharedPtr<FActiveTimerHandle>().ToSharedRef();
	}

	/** @return a reference to the list of selected graph nodes */
	virtual const FGraphPanelSelectionSet& GetSelectedNodes() const
	{
		static FGraphPanelSelectionSet NoSelection;

		if (Implementation.IsValid())
		{
			return Implementation->GetSelectedNodes();
		}
		else
		{
			return NoSelection;
		}
	}

	/** Clear the selection */
	virtual void ClearSelectionSet()
	{
		if (Implementation.IsValid())
		{
			Implementation->ClearSelectionSet();
		}
	}

	/** Set the selection status of a node */
	virtual void SetNodeSelection(UEdGraphNode* Node, bool bSelect)
	{
		if (Implementation.IsValid())
		{
			Implementation->SetNodeSelection(Node, bSelect);
		}
	}
	
	/** Select all nodes */
	virtual void SelectAllNodes()
	{
		if (Implementation.IsValid())
		{
			Implementation->SelectAllNodes();
		}		
	}

	virtual class UEdGraphPin* GetGraphPinForMenu()
	{
		if ( Implementation.IsValid() )
		{
			return Implementation->GetGraphPinForMenu();
		}
		else
		{
			return NULL;
		}
	}

	virtual class UEdGraphNode* GetGraphNodeForMenu()
	{
		if ( Implementation.IsValid() )
		{
			return Implementation->GetGraphNodeForMenu();
		}
		else
		{
			return NULL;
		}
	}

	// Zooms out to fit either all nodes or only the selected ones
	virtual void ZoomToFit(bool bOnlySelection)
	{
		if (Implementation.IsValid())
		{
			return Implementation->ZoomToFit(bOnlySelection);
		}
	}

	/** Get Bounds for selected nodes, false if nothing selected*/
	virtual bool GetBoundsForSelectedNodes( class FSlateRect& Rect, float Padding  )
	{
		if (Implementation.IsValid())
		{
			return Implementation->GetBoundsForSelectedNodes(Rect, Padding);
		}
		return false;
	}

	/** Get Bounds for the specified node, returns false on failure */
	virtual bool GetBoundsForNode( const UEdGraphNode* InNode, class FSlateRect& Rect, float Padding ) const
	{
		if (Implementation.IsValid())
		{
			return Implementation->GetBoundsForNode(InNode, Rect, Padding);
		}
		return false;
	}

	virtual void StraightenConnections()
	{
		if (Implementation.IsValid())
		{
			return Implementation->StraightenConnections();
		}
	}

	virtual void StraightenConnections(UEdGraphPin* SourcePin, UEdGraphPin* PinToAlign = nullptr) const
	{
		if (Implementation.IsValid())
		{
			return Implementation->StraightenConnections(SourcePin, PinToAlign);
		}
	}

	virtual void RefreshNode(UEdGraphNode& Node)
	{
		if (Implementation.IsValid())
		{
			return Implementation->RefreshNode(Node);
		}
	}
	
	/** Invoked when the Graph being edited changes in some way. */
	virtual void NotifyGraphChanged()
	{
		if (Implementation.IsValid())
		{
			Implementation->NotifyGraphChanged();
		}
	}

	/* Get the title bar if there is one */
	virtual TSharedPtr<SWidget> GetTitleBar() const
	{
		if (Implementation.IsValid())
		{
			return Implementation->GetTitleBar();
		}
		return TSharedPtr<SWidget>();
	}

	/** Show notification on graph */
	virtual void AddNotification(FNotificationInfo& Info, bool bSuccess)
	{
		if (Implementation.IsValid())
		{
			Implementation->AddNotification(Info, bSuccess);
		}
	}

	/** Capture keyboard */
	virtual void CaptureKeyboard()
	{
		if (Implementation.IsValid())
		{
			Implementation->CaptureKeyboard();
		}
	}

	/** Sets the current node, pin and connection factory. */
	virtual void SetNodeFactory(const TSharedRef<class FGraphNodeFactory>& NewNodeFactory)
	{
		if (Implementation.IsValid())
		{
			Implementation->SetNodeFactory(NewNodeFactory);
		}
	}
	
	virtual void OnCollapseNodes()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnCollapseNodes();
		}
	}

	virtual bool CanCollapseNodes() const
	{
		return Implementation.IsValid() ? Implementation->CanCollapseNodes() : false;
	}

	virtual void OnExpandNodes()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnExpandNodes();
		}
	}

	virtual bool CanExpandNodes() const
	{
		return Implementation.IsValid() ? Implementation->CanExpandNodes() : false;
	}

	virtual void OnAlignTop()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnAlignTop();
		}
	}

	virtual void OnAlignMiddle()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnAlignMiddle();
		}
	}

	virtual void OnAlignBottom()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnAlignBottom();
		}
	}

	virtual void OnAlignLeft()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnAlignLeft();
		}
	}

	virtual void OnAlignCenter()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnAlignCenter();
		}
	}

	virtual void OnAlignRight()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnAlignRight();
		}
	}


	virtual void OnStraightenConnections()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnStraightenConnections();
		}
	}


	virtual void OnDistributeNodesH()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnDistributeNodesH();
		}
	}

	virtual void OnDistributeNodesV()
	{
		if (Implementation.IsValid())
		{
			Implementation->OnDistributeNodesV();
		}
	}


	virtual int32 GetNumberOfSelectedNodes() const
	{
		if (Implementation.IsValid())
		{
			return Implementation->GetNumberOfSelectedNodes();
		}
		return 0;
	}


	/** Returns the currently selected node if there is a single node selected (if there are multiple nodes selected or none selected, it will return nullptr) */
	virtual UEdGraphNode* GetSingleSelectedNode() const
	{
		if (Implementation.IsValid())
		{
			return Implementation->GetSingleSelectedNode();
		}
		return nullptr;
	}
	

	/** Returns the graph panel used for this graph editor */
	virtual SGraphPanel* GetGraphPanel() const
	{
		if (Implementation.IsValid())
		{
			return Implementation->GetGraphPanel();
		}
		return nullptr;
	}

	/** Invoked when the underlying Graph is being changed. */
	virtual void OnGraphChanged(const struct FEdGraphEditAction& InAction) override
	{
		if (Implementation.IsValid())
		{
			Implementation->OnGraphChanged(InAction);
		}
	}

private:
	static void RegisterGraphEditor(const TSharedRef<SGraphEditor>& InGraphEditor);

	void ConstructImplementation( const FArguments& InArgs );

	TSharedPtr<SGraphEditor_PMS> Implementation;
};
