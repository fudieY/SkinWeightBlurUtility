#include "skinweight.h"
#include <cmath>
SkinWeight SkinWeight::operator + (SkinWeight a) {
	SkinWeight result;
	result.id = id;
	result.pos = pos;
	result.nodes = nodes;
	result.weights = weights;
	for (int i = 0; i < a.nodes.Count(); i++) {
		bool findNode = false;
		for (int j = 0; j < result.nodes.Count(); j++) {
			if (a.nodes[i] == result.nodes[j]) {
				result.nodes.Count();
				findNode = true;
				result.weights[j] += a.weights[i];
			}

		}
		if (!findNode) {
			int num = result.nodes.Count(); 
			result.nodes.SetCount(num + 1);
			result.nodes[num] = a.nodes[i];
			result.weights.SetCount(num + 1 );
			result.weights[num] = a.weights[i];
		}
	}
	return result;
}

SkinWeight* SkinWeight::operator += (SkinWeight a) {
	for (int i = 0; i < a.nodes.Count(); i++) {
		bool findNode = false;
		for (int j = 0; j < nodes.Count(); j++) {
			INode* nodea = a.nodes[i];
			INode* nodeb = this->nodes[j]; 
			if (a.nodes[i] == this->nodes[j]) {
				this->nodes.Count();
				findNode = true;
				this->weights[j] += a.weights[i];
			}

		}
		if (!findNode) {
			int num = this->nodes.Count(); 
			this->nodes.SetCount(num + 1 );
			this->nodes[num] = a.nodes[i];
			this->weights.SetCount(num + 1 );
			this->weights[num] = a.weights[i];
		}
	}
	return this;
}

SkinWeight SkinWeight::operator * (float num) {
	SkinWeight result;
	result.id = id;
	result.pos = pos;
	result.nodes = nodes;
	result.weights = weights;
	for (int i = 0; i < nodes.Count(); i++) {			
			result.weights[i] = result.weights[i] * num;		
	}
	return result;
}
void SkinWeight::Normalize() {
	float num =0; 
	for (int i = 0; i < weights.Count(); i++) {
		num += weights[i];
	}
	for (int i = 0; i < weights.Count(); i++) {
		weights[i] /= num;
	}
}

SkinWeight GetSkinweight(INode* node, Object* baseobject, Modifier* skinmod, int vertexID) {
	SkinWeight result;
	result.id = vertexID; 
	ISkin* skin = (ISkin*)skinmod->GetInterface(I_SKIN);
	ISkinContextData* skinConstData = skin->GetContextInterface(node);
	ISkinImportData* skinImportData = (ISkinImportData*)skinmod->GetInterface(I_SKINIMPORTDATA);

	bool vertexIDcheck = skinConstData->GetNumPoints() > vertexID;
	if (vertexIDcheck) {
		TriObject* triobj = (TriObject*)baseobject->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
		result.pos = triobj->GetPoint(vertexID);
		int bonecount = skinConstData->GetNumAssignedBones(vertexID);
		result.nodes.SetCount(bonecount);
		result.weights.SetCount(bonecount);
		for (int i = 0; i < bonecount; i++) {
			int boneid = skinConstData->GetAssignedBone(vertexID, i);
			INode* a = skin->GetBone(boneid);
			result.nodes[i] = a;
			result.weights[i] = skinConstData->GetBoneWeight(vertexID, i);
		}
	}
	return result;
}

std::vector<SkinWeight>  GetSkinweightArray(INode* node, Object* baseobject, Modifier* skinmod) {
	ISkin* skin = (ISkin*)skinmod->GetInterface(I_SKIN);
	ISkinContextData* skinConstData = skin->GetContextInterface(node);
	ISkinImportData* skinImportData = (ISkinImportData*)skinmod->GetInterface(I_SKINIMPORTDATA);

	TriObject* triobj = (TriObject*)baseobject->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
	int pcount = triobj->mesh.getNumVerts();
	std::vector<SkinWeight> result(pcount);
	
	for (int vertexID = 0; vertexID < pcount; vertexID++) {
		SkinWeight skinweight;
		skinweight.id = vertexID; 
		skinweight.pos = triobj->GetPoint(vertexID);
		int bonecount = skinConstData->GetNumAssignedBones(vertexID);
		skinweight.nodes.SetCount(bonecount);
		skinweight.weights.SetCount(bonecount);
		for (int i = 0; i < bonecount; i++) {
			int boneid = skinConstData->GetAssignedBone(vertexID, i);
			skinweight.nodes[i] = skin->GetBone(boneid);
			skinweight.weights[i] = skinConstData->GetBoneWeight(vertexID, i);
		}
		result[vertexID] = skinweight;
	}
	return result;
}

SkinWeight BlurSkinWeightByDistance(std::vector<SkinWeight>* skinArray ,int vertexID,float minDistance ,float maxDistance ) {
	SkinWeight result; 
	float leapDistance = maxDistance - minDistance;
	for (int i = 0; i < skinArray->size(); i++) {
		float distance = (skinArray->at(i).pos - skinArray->at(vertexID).pos).Length();
		if (distance <= minDistance) {
			result += skinArray->at(i);
		}
		else if (distance <= maxDistance) {
			float num = 1.0 - (distance - minDistance) / leapDistance;
			result += skinArray->at(i) * num;
		}
	}
	result.id = vertexID; 
	return result; 
}
void SetWeightBySkinWeight(INode* node,ISkinImportData* skinimportdata , SkinWeight skinweight) {
	skinimportdata->AddWeights(node, skinweight.id, skinweight.nodes, skinweight.weights);
}