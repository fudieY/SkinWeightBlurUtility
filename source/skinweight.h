#pragma once

#include "3dsmaxsdk_preinclude.h"
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
#include <iskin.h>
#include <modstack.h>
#include <triobj.h>
#include <vector>

class SkinWeight {
public :
	int id; 
	Tab<INode*> nodes; 
	Tab<float> weights; 
	Point3 pos; 
	SkinWeight operator + (SkinWeight a);
	SkinWeight operator * (float num);
	SkinWeight* operator += (SkinWeight a);
	void Normalize();
};

SkinWeight GetSkinweight(INode* node, Object* baseobject, Modifier* skinmod, int vertexID);
std::vector<SkinWeight>  GetSkinweightArray(INode* node, Object* baseobject, Modifier* skinmod);
SkinWeight BlurSkinWeightByDistance(std::vector<SkinWeight>* skinArray, int vertexID, float minDistance, float maxDistance);
void SetWeightBySkinWeight(INode* node, ISkinImportData* skinimportdata, SkinWeight skinweight);