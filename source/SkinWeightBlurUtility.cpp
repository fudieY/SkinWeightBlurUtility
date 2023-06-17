//**************************************************************************/
// Copyright (c) 1998-2020 Autodesk, Inc.
// All rights reserved.
// 
// Use of this software is subject to the terms of the Autodesk license 
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.
//**************************************************************************/
// DESCRIPTION: Plugin Wizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "SkinWeightBlurUtility.h"

#include "ui_plugin_form.h"
#include "qmessagebox.h"
#include "qobject.h"
#include <time.h>
#include <modstack.h>
#include <triobj.h>
#include <iskin.h>
#include <vector>
#include <wchar.h>
#include "skinweight.h"
#include <iostream>

#define SkinWeightBlurUtility_CLASS_ID Class_ID(0x38ea0dd1, 0x73d0f280)

class SkinWeightBlurUtility : public UtilityObj, public QObject{
public:
	// Constructor/Destructor
	SkinWeightBlurUtility();
	virtual ~SkinWeightBlurUtility();

	void DeleteThis() override {}

	void BeginEditParams(Interface* ip, IUtil* iu) override;
	void EndEditParams(Interface* ip, IUtil* iu) override;

	virtual void Init(HWND hWnd);
	virtual void Destroy(HWND hWnd);

	// Singleton access
	static SkinWeightBlurUtility* GetInstance()
	{
		static SkinWeightBlurUtility theSkinWeightBlurUtility;
		return &theSkinWeightBlurUtility;
	}

private:
	void DoSomething();
	QWidget* widget;
	Ui::PluginRollup ui;

	IUtil* iu;
	Interface* ip;
};


class SkinWeightBlurUtilityClassDesc : public ClassDesc2 
{
public:
	int           IsPublic() override                               { return TRUE; }
	void*         Create(BOOL /*loading = FALSE*/) override         { return SkinWeightBlurUtility::GetInstance(); }
	const TCHAR*  ClassName() override                              { return GetString(IDS_CLASS_NAME); }
	const TCHAR*  NonLocalizedClassName() override                  { return _T("SkinWeightBlurUtility"); }
	SClass_ID     SuperClassID() override                           { return UTILITY_CLASS_ID; }
	Class_ID      ClassID() override                                { return SkinWeightBlurUtility_CLASS_ID; }
	const TCHAR*  Category() override                               { return GetString(IDS_CATEGORY); }

	const TCHAR*  InternalName() override                           { return _T("SkinWeightBlurUtility"); } // Returns fixed parsable name (scripter-visible name)
	HINSTANCE     HInstance() override                              { return hInstance; } // Returns owning module handle


};

ClassDesc2* GetSkinWeightBlurUtilityDesc()
{
	static SkinWeightBlurUtilityClassDesc SkinWeightBlurUtilityDesc;
	return &SkinWeightBlurUtilityDesc; 
}




//--- SkinWeightBlurUtility -------------------------------------------------------
SkinWeightBlurUtility::SkinWeightBlurUtility()
	: iu(nullptr)
{

}

SkinWeightBlurUtility::~SkinWeightBlurUtility()
{

}

void SkinWeightBlurUtility::BeginEditParams(Interface* ip,IUtil* iu)
{
	this->iu = iu;
	widget = new QWidget;
	ui.setupUi(widget);

	// We can connect UI signals here using Qt Functor syntax
	QObject::connect(ui.pushButton, &QPushButton::clicked, this, &SkinWeightBlurUtility::DoSomething);
	ip->AddRollupPage(*widget, L"Plug-in Rollup");
	this->ip = ip; 
}

void SkinWeightBlurUtility::EndEditParams(Interface* ip, IUtil*)
{
	this->iu = nullptr;
	ip -> DeleteRollupPage(*widget);
}

void SkinWeightBlurUtility::Init(HWND /*handle*/)
{

}

void SkinWeightBlurUtility::Destroy(HWND /*handle*/)
{

}

void SkinWeightBlurUtility::DoSomething()
{
	if (ip->GetSelNodeCount() > 0) {
		IDerivedObject* iDerivedobj = dynamic_cast<IDerivedObject*>(ip->GetSelNode(0)->GetObjectRef());
		INode* node = ip->GetSelNode(0);
		if (iDerivedobj != nullptr) {
			int modCount = iDerivedobj->NumModifiers();
			Modifier* skinmod = nullptr;
			ISkin* skin = nullptr;
			int skin_classID_A = 9815843, skin_classID_B = 87654;
			for (int i = 0; i < modCount; i++) {
				if (iDerivedobj->GetModifier(i)->ClassID() == Class_ID(skin_classID_A, skin_classID_B)) {
					skinmod = (Modifier*)iDerivedobj->GetModifier(i);
					skin = (ISkin*)skinmod->GetInterface(I_SKIN);

				}
			}
			if (skin != nullptr) {


				DebugPrint(_M("bonecount %ls \n"), skin->GetBoneName(0));


				ISkinContextData* SkinCdata = skin->GetContextInterface(node);
				ISkinImportData* skinImportdata = (ISkinImportData*)skinmod->GetInterface(I_SKINIMPORTDATA);

				int pcount = SkinCdata->GetNumPoints();
				Object* obj = node->GetObjectRef()->FindBaseObject();
				BitArray sel;
				ISkin2* skin2 = (ISkin2*)skinmod->GetInterface(I_SKIN2);
				skin2->GetVertexSelection(node, sel);
				if (pcount > 0) {
					std::vector<SkinWeight> wtArray = GetSkinweightArray(node, obj, skinmod);
					SkinWeight skinweight = BlurSkinWeightByDistance(&wtArray, 0, ui.MinSpinBox->value(), ui.MaxSpinBox->value());
					//DebugPrint(_M("variable:%d	variable:%d	 variable:%d \n"), skinweight.id, skinweight.nodes.Count(), skinweight.weights.Count());

					for (int i = 0; i < sel.GetSize(); i++) {
						if (sel[i]) {
							SkinWeight skinweight = BlurSkinWeightByDistance(&wtArray, i, ui.MinSpinBox->value(), ui.MaxSpinBox->value());
							//DebugPrint(_M("variable:%d	variable:%d	 variable:%d \n"), skinweight.id, skinweight.nodes.Count(), skinweight.weights.Count());
							SetWeightBySkinWeight(node, skinImportdata, skinweight);
						}

					}


				}
			}
		}

	}
	QMessageBox::information(widget, "Skin Weight Blur ", QString("complated skin weight blur "));
}
