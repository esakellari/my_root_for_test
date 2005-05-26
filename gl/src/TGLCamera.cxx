// @(#)root/gl:$Name:$:$Id:$
// Author:  Richard Maunder  25/05/2005
// Parts taken from original by Timur Pocheptsov

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

// TODO: Function descriptions
// TODO: Class def - same as header

#include "TGLCamera.h"
#include "TGLIncludes.h"
#include "TGLBoundingBox.h"
#include "TError.h"

ClassImp(TGLCamera)

//______________________________________________________________________________
TGLCamera::TGLCamera() :
   fViewport(0,0,100,100),
   fProjM(),  fModVM(), fClipM(),
   fCacheDirty(true),
   fInterestBox(), fLargestInterest(0.0)
{
}

//______________________________________________________________________________
TGLCamera::~TGLCamera()
{}

//______________________________________________________________________________
void TGLCamera::SetViewport(const TGLRect & viewport)
{
   fViewport = viewport;
   fCacheDirty = true;
}

//______________________________________________________________________________
void TGLCamera::UpdateCache()
{
   // Update internal cached values
   assert(fCacheDirty);

   glGetDoublev(GL_PROJECTION_MATRIX, fProjM.Arr());
   glGetDoublev(GL_MODELVIEW_MATRIX, fModVM.Arr());

   // Multiply projection by modelview to get the clip matrix
   // TODO: Move this into TGLMatrix or shift all over to ROOT ones
   fClipM[ 0] = fModVM[ 0] * fProjM[ 0] + fModVM[ 1] * fProjM[ 4] + fModVM[ 2] * fProjM[ 8] + fModVM[ 3] * fProjM[12];
   fClipM[ 1] = fModVM[ 0] * fProjM[ 1] + fModVM[ 1] * fProjM[ 5] + fModVM[ 2] * fProjM[ 9] + fModVM[ 3] * fProjM[13];
   fClipM[ 2] = fModVM[ 0] * fProjM[ 2] + fModVM[ 1] * fProjM[ 6] + fModVM[ 2] * fProjM[10] + fModVM[ 3] * fProjM[14];
   fClipM[ 3] = fModVM[ 0] * fProjM[ 3] + fModVM[ 1] * fProjM[ 7] + fModVM[ 2] * fProjM[11] + fModVM[ 3] * fProjM[15];

   fClipM[ 4] = fModVM[ 4] * fProjM[ 0] + fModVM[ 5] * fProjM[ 4] + fModVM[ 6] * fProjM[ 8] + fModVM[ 7] * fProjM[12];
   fClipM[ 5] = fModVM[ 4] * fProjM[ 1] + fModVM[ 5] * fProjM[ 5] + fModVM[ 6] * fProjM[ 9] + fModVM[ 7] * fProjM[13];
   fClipM[ 6] = fModVM[ 4] * fProjM[ 2] + fModVM[ 5] * fProjM[ 6] + fModVM[ 6] * fProjM[10] + fModVM[ 7] * fProjM[14];
   fClipM[ 7] = fModVM[ 4] * fProjM[ 3] + fModVM[ 5] * fProjM[ 7] + fModVM[ 6] * fProjM[11] + fModVM[ 7] * fProjM[15];

   fClipM[ 8] = fModVM[ 8] * fProjM[ 0] + fModVM[ 9] * fProjM[ 4] + fModVM[10] * fProjM[ 8] + fModVM[11] * fProjM[12];
   fClipM[ 9] = fModVM[ 8] * fProjM[ 1] + fModVM[ 9] * fProjM[ 5] + fModVM[10] * fProjM[ 9] + fModVM[11] * fProjM[13];
   fClipM[10] = fModVM[ 8] * fProjM[ 2] + fModVM[ 9] * fProjM[ 6] + fModVM[10] * fProjM[10] + fModVM[11] * fProjM[14];
   fClipM[11] = fModVM[ 8] * fProjM[ 3] + fModVM[ 9] * fProjM[ 7] + fModVM[10] * fProjM[11] + fModVM[11] * fProjM[15];

   fClipM[12] = fModVM[12] * fProjM[ 0] + fModVM[13] * fProjM[ 4] + fModVM[14] * fProjM[ 8] + fModVM[15] * fProjM[12];
   fClipM[13] = fModVM[12] * fProjM[ 1] + fModVM[13] * fProjM[ 5] + fModVM[14] * fProjM[ 9] + fModVM[15] * fProjM[13];
   fClipM[14] = fModVM[12] * fProjM[ 2] + fModVM[13] * fProjM[ 6] + fModVM[14] * fProjM[10] + fModVM[15] * fProjM[14];
   fClipM[15] = fModVM[12] * fProjM[ 3] + fModVM[13] * fProjM[ 7] + fModVM[14] * fProjM[11] + fModVM[15] * fProjM[15];

   // RIGHT clipping plane
   fFrustumPlanes[kRIGHT].Set(fClipM[ 3] - fClipM[ 0],
                              fClipM[ 7] - fClipM[ 4],
                              fClipM[11] - fClipM[ 8],
                              fClipM[15] - fClipM[12]);

   // LEFT clipping plane
   fFrustumPlanes[kLEFT].Set(fClipM[ 3] + fClipM[ 0],
                             fClipM[ 7] + fClipM[ 4],
                             fClipM[11] + fClipM[ 8],
                             fClipM[15] + fClipM[12]);

   // BOTTOM clipping plane
   fFrustumPlanes[kBOTTOM].Set(fClipM[ 3] + fClipM[ 1],
                               fClipM[ 7] + fClipM[ 5],
                               fClipM[11] + fClipM[ 9],
                               fClipM[15] + fClipM[13]);


   // TOP clipping plane
   fFrustumPlanes[kTOP].Set(fClipM[ 3] - fClipM[ 1],
                            fClipM[ 7] - fClipM[ 5],
                            fClipM[11] - fClipM[ 9],
                            fClipM[15] - fClipM[13]);

   // FAR clipping plane
   fFrustumPlanes[kFAR].Set(fClipM[ 3] - fClipM[ 2],
                            fClipM[ 7] - fClipM[ 6],
                            fClipM[11] - fClipM[10],
                            fClipM[15] - fClipM[14]);

   // NEAR clipping plane
   fFrustumPlanes[kNEAR].Set(fClipM[ 3] + fClipM[ 2],
                             fClipM[ 7] + fClipM[ 6],
                             fClipM[11] + fClipM[10],
                             fClipM[15] + fClipM[14]);

   fCacheDirty = kFALSE;
}

//______________________________________________________________________________
TGLBoundingBox TGLCamera::FrustumBox() const
{
   if (fCacheDirty) {
      Error("TGLCamera::FrustumBox(", "cached dirty");
   }

   TGLVertex3 vertex[8];

   //   7-------6
   //  /|      /|
   // 3-------2 |
   // | 4-----|-5
   // |/      |/
   // 0-------1

   // Get four vertices of frustum on the far clipping plane
   vertex[4] = Intersection(fFrustumPlanes[kFAR], fFrustumPlanes[kBOTTOM], fFrustumPlanes[kLEFT]);
   vertex[5] = Intersection(fFrustumPlanes[kFAR], fFrustumPlanes[kBOTTOM], fFrustumPlanes[kRIGHT]);
   vertex[6] = Intersection(fFrustumPlanes[kFAR], fFrustumPlanes[kTOP], fFrustumPlanes[kRIGHT]);
   vertex[7] = Intersection(fFrustumPlanes[kFAR], fFrustumPlanes[kTOP], fFrustumPlanes[kLEFT]);

   // Now find the matching four verticies for above, which are the nearest ones on near clip plane
   // As near and far clip planes are parallel this forms a orientated box encompassing the frustum
   vertex[0] = fFrustumPlanes[kNEAR].NearestOn(vertex[4]);
   vertex[1] = fFrustumPlanes[kNEAR].NearestOn(vertex[5]);
   vertex[2] = fFrustumPlanes[kNEAR].NearestOn(vertex[6]);
   vertex[3] = fFrustumPlanes[kNEAR].NearestOn(vertex[7]);

   return TGLBoundingBox(vertex);
}

//______________________________________________________________________________
EOverlap TGLCamera::FrustumOverlap(const TGLBoundingBox & box) const
{
   if (fCacheDirty) {
      Error("TGLCamera::FrustumBox(", "cached dirty");
   }

   // Test shape against each plane in frustum - returning overlap result
   // This method can result in kFALSE positives, where shape lies outside
   // frustum, but not outside a single plane of it. In this case the shape
   // will be regarded incorrectly as intersecting (kPartial)
   // TODO: Improve this - have a reliable test (seperating axes).

   Int_t PlanesInside = 0; // Assume outside to start
   for (Int_t PlaneIndex = 0; PlaneIndex < kPLANESPERFRUSTUM; ++PlaneIndex) {
      EOverlap planeOverlap = box.Overlap(fFrustumPlanes[PlaneIndex]);

	  // Special case - any object which comes through the near clipping
     // plane is completely removed - disabled at present
     // TODO: In future may want to fade object (opacity) as they approach
      // near clip - how will this be returned? template pair?
      /*if (PlaneIndex == kNEAR && planeOverlap == kPartial) {
         return kOutside;
      }*/
      // Once we find a single plane which shape is outside, we are outside the frustum
      if ( planeOverlap == kOutside ) {
         return kOutside;
      } else if ( planeOverlap == kInside ) {
         PlanesInside++;
      }
   }
   // Completely inside frustum
   if ( PlanesInside == kPLANESPERFRUSTUM ) {
      return kInside;
   } else {
      return kPartial;
   }
}

//______________________________________________________________________________
EOverlap TGLCamera::ViewportOverlap(const TGLBoundingBox & box) const
{
   // No cached values need here
   return ViewportSize(box).Overlap(fViewport);
}

//______________________________________________________________________________
TGLRect TGLCamera::ViewportSize(const TGLBoundingBox & box) const
{
   if (fCacheDirty) {
      Error("TGLCamera::FrustumBox(", "cached dirty");
   }

   // May often result in a rect bigger then the viewport
   // as gluProject does not clip.
   Double_t winX, winY, winZ;
   TGLRect  screenRect;

   // Find the projection of the 8 vertexs of the bounding box onto screen
   // and the enclosing rect round these.
   for (Int_t i = 0; i < 8; i++)
   {
      const TGLVertex3 & vertex = box[i];

      //TODO: Convert TGLRect so this not required
      GLint viewport[4] = { fViewport.X(), fViewport.Y(), fViewport.Width(), fViewport.Height() };

      gluProject(vertex.X(), vertex.Y(), vertex.Z(), fModVM.CArr(), fProjM.CArr(), viewport, &winX, &winY, &winZ);

      if (i == 0) {
         screenRect.SetCorner(static_cast<Int_t>(winX),static_cast<Int_t>(winY));
      }
      else {
         screenRect.Expand(static_cast<Int_t>(winX), static_cast<Int_t>(winY));
      }
   }

   return screenRect;
}

//______________________________________________________________________________
Bool_t TGLCamera::OfInterest(const TGLBoundingBox & box) const
{
   Bool_t interest = kFALSE;

   // If interest box is empty we take everything with volume larger than
   // 1% of largest seen so far
   if (fInterestBox.IsEmpty()) {
      if (box.Volume() >= fLargestInterest * 0.01) {
         if (box.Volume() > fLargestInterest) {
            fLargestInterest = box.Volume();
         }
         interest = kTRUE;
      }
   } else {
      if ((box.Extents().Mag() / fInterestBox.Extents().Mag() > 0.001) ||
          (box.Volume() / fInterestBox.Volume() > 0.005)) {
         interest = fInterestBox.Intersect(box);
      }
   }

   return interest;
}

//______________________________________________________________________________
Bool_t TGLCamera::UpdateInterest()
{
   Bool_t exposedUpdate = kFALSE;

   TGLBoundingBox frustumBox = FrustumBox();
   Double_t ratio = fInterestBox.Volume() / (frustumBox.Volume() * 8.0); // 2^3
   if (fInterestBox.IsEmpty() || !fInterestBox.AlignedContains(frustumBox) ||
       ratio > 8.0 || ratio < 0.125 ) {

	   //std::cout << "UpdateInterest: Ratio: " << ratio;
      //if(fInterestBox.IsEmpty()) { std::cout << "Interest EMPTY"; }
      //if(fInterestBox.AlignedContains(frustumBox)) { std::cout << "FrustIN"; } else { std::cout << "FrustOUT"; }
	   //std::cout << std::endl << "Frustum : " << std::endl; frustumBox.Dump();
	   //std::cout << "Old Interest : " << std::endl; fInterestBox.Dump();
      // Construct a new axis aligned interest box based on limits of camera frustum
      TGLVertex3 low(frustumBox.XMin(), frustumBox.YMin(), frustumBox.ZMin());
      TGLVertex3 high(frustumBox.XMax(), frustumBox.YMax(), frustumBox.ZMax());
      fInterestBox.SetAligned(low, high);
      //assert(fInterestBox.AlignedContains(frustumBox) && fInterestBox.Volume() >= frustumBox.Volume());
      fInterestBox.Scale(2.0);

	   //std::cout << "New Interest : " << std::endl; fInterestBox.Dump();
      exposedUpdate = kTRUE;
   }

   return exposedUpdate;
}

//______________________________________________________________________________
void TGLCamera::ResetInterest()
{
   fInterestBox.SetEmpty();
   fLargestInterest = 0.0;
}

//______________________________________________________________________________
Bool_t TGLCamera::AdjustAndClampVal(Double_t & val, Double_t min, Double_t max,
                                    Int_t shift, Int_t shiftRange) const
{
   Double_t oldVal = val;

   val -= (Double_t)shift * (max - min) / shiftRange;
   if (val < min) {
      val = min;
   }
   else if (val > max) {
      val = max;
   }

   if (val != oldVal)
   {
      return kTRUE;
   }
   else
   {
      return kFALSE;
   }
}
