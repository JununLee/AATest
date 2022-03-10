/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageActorPointPlacerLTY.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

	 This software is distributed WITHOUT ANY WARRANTY; without even
	 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//#include "pch.h"
#include "vtkImageActorPointPlacerLTY.h"
#include "vtkObjectFactory.h"
#include "vtkBoundedPlanePointPlacer.h"
#include "vtkPlane.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapper3D.h"

vtkStandardNewMacro(vtkImageActorPointPlacerLTY);

vtkCxxSetObjectMacro(vtkImageActorPointPlacerLTY, ImageActor, vtkImageActor);

//----------------------------------------------------------------------
vtkImageActorPointPlacerLTY::vtkImageActorPointPlacerLTY()
{
	this->Placer = vtkBoundedPlanePointPlacer::New();
	this->ImageActor = nullptr;
	this->SavedBounds[0] = 0.0;
	this->SavedBounds[1] = 0.0;
	this->SavedBounds[2] = 0.0;
	this->SavedBounds[3] = 0.0;
	this->SavedBounds[4] = 0.0;
	this->SavedBounds[5] = 0.0;
	this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = VTK_DOUBLE_MAX;
	this->Bounds[1] = this->Bounds[3] = this->Bounds[5] = VTK_DOUBLE_MIN;
}

//----------------------------------------------------------------------
vtkImageActorPointPlacerLTY::~vtkImageActorPointPlacerLTY()
{
	this->Placer->Delete();
	this->SetImageActor(nullptr);
}


//----------------------------------------------------------------------
int vtkImageActorPointPlacerLTY::ComputeWorldPosition(vtkRenderer *ren,
	double  displayPos[2],
	double *refWorldPos,
	double  worldPos[3],
	double  worldOrient[9])
{
	if (!this->UpdateInternalState())
	{
		return 0;
	}

	return this->Placer->ComputeWorldPosition(ren, displayPos,
		refWorldPos, worldPos,
		worldOrient);
}

//----------------------------------------------------------------------
int vtkImageActorPointPlacerLTY::ComputeWorldPosition(vtkRenderer *ren,
	double displayPos[2],
	double worldPos[3],
	double worldOrient[9])
{
	if (!this->UpdateInternalState())
	{
		return 0;
	}

	return this->Placer->ComputeWorldPosition(ren, displayPos, worldPos, worldOrient);
}

//----------------------------------------------------------------------
int vtkImageActorPointPlacerLTY::ValidateWorldPosition(double worldPos[3],
	double *worldOrient)
{
	if (!this->UpdateInternalState())
	{
		return 0;
	}

	return this->Placer->ValidateWorldPosition(worldPos, worldOrient);
}

//----------------------------------------------------------------------
int vtkImageActorPointPlacerLTY::ValidateWorldPosition(double worldPos[3])
{
	if (!this->UpdateInternalState())
	{
		return 0;
	}

	return this->Placer->ValidateWorldPosition(worldPos);
}

//----------------------------------------------------------------------
int vtkImageActorPointPlacerLTY::UpdateWorldPosition(vtkRenderer *ren,
	double worldPos[3],
	double worldOrient[9])
{
	if (!this->UpdateInternalState())
	{
		return 0;
	}

	return this->Placer->UpdateWorldPosition(ren,
		worldPos,
		worldOrient);
}

//----------------------------------------------------------------------
int vtkImageActorPointPlacerLTY::UpdateInternalState()
{
	if (!this->ImageActor)
	{
		return 0;
	}

	vtkPlane *plane;
	plane = this->ImageActor->GetMapper()->GetSlicePlane();
	if (!plane)
	{
		return 0;
	}

	vtkImageData *image;
	image = this->ImageActor->GetInput();
	if (!image)
	{
		return 0;
	}


	double spacing[3];
	image->GetSpacing(spacing);

	double origin[3];
	image->GetOrigin(origin);

	double bounds[6];
	this->ImageActor->GetBounds(bounds);


	if (this->Bounds[0] != VTK_DOUBLE_MAX)
	{
		bounds[0] = (bounds[0] < this->Bounds[0]) ? this->Bounds[0] : bounds[0];
		bounds[1] = (bounds[1] > this->Bounds[1]) ? this->Bounds[1] : bounds[1];
		bounds[2] = (bounds[2] < this->Bounds[2]) ? this->Bounds[2] : bounds[2];
		bounds[3] = (bounds[3] > this->Bounds[3]) ? this->Bounds[3] : bounds[3];
		bounds[4] = (bounds[4] < this->Bounds[4]) ? this->Bounds[4] : bounds[4];
		bounds[5] = (bounds[5] > this->Bounds[5]) ? this->Bounds[5] : bounds[5];
	}


	this->Placer->SetProjectionNormalToOblique();
	this->Placer->SetObliquePlane(plane);

	this->SavedBounds[0] = bounds[0];
	this->SavedBounds[1] = bounds[1];
	this->SavedBounds[2] = bounds[2];
	this->SavedBounds[3] = bounds[3];
	this->SavedBounds[4] = bounds[4];
	this->SavedBounds[5] = bounds[5];
	this->Modified();

	return 1;
}

//----------------------------------------------------------------------
void vtkImageActorPointPlacerLTY::SetWorldTolerance(double tol)
{
	if (this->WorldTolerance !=
		(tol < 0.0 ? 0.0 : (tol > VTK_DOUBLE_MAX ? VTK_DOUBLE_MAX : tol)))
	{
		this->WorldTolerance =
			(tol < 0.0 ? 0.0 : (tol > VTK_DOUBLE_MAX ? VTK_DOUBLE_MAX : tol));
		this->Placer->SetWorldTolerance(tol);
		this->Modified();
	}
}

//----------------------------------------------------------------------
void vtkImageActorPointPlacerLTY::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	const double *bounds = this->GetBounds();
	if (bounds != nullptr)
	{
		os << indent << "Bounds: \n";
		os << indent << "  Xmin,Xmax: ("
			<< this->Bounds[0] << ", " << this->Bounds[1] << ")\n";
		os << indent << "  Ymin,Ymax: ("
			<< this->Bounds[2] << ", " << this->Bounds[3] << ")\n";
		os << indent << "  Zmin,Zmax: ("
			<< this->Bounds[4] << ", " << this->Bounds[5] << ")\n";
	}
	else
	{
		os << indent << "Bounds: (not defined)\n";
	}

	os << indent << "Image Actor: " << this->ImageActor << "\n";
}

