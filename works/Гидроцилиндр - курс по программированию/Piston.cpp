
#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

Ptr<Application> app;
Ptr<UserInterface> ui;

extern "C" XI_EXPORT bool run(const char* context)
{
	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
	if (!ui)
		return false;

	//��������� �� ��������
	Ptr<Documents> docs = app->documents();
	if (!docs) return false;
	Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
	if (!doc) return false;
	// ��������� �� ����������� �� �������� ���������
	Ptr<Design> design = app->activeProduct();
	if (!design) return false;
	//��������� �� ���������
	Ptr<Component> rootComp = design->rootComponent();
	if (!rootComp) return false;
	//��������� �� �������
	Ptr<Features> features = rootComp->features();
	if (!features) return false;
	//��������� �� �����
	Ptr<Sketches> sketches = rootComp->sketches();
	if (!sketches) return false;
	// ��������� �� ������������
	Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
	if (!extrudes) return false;
	//��������� �� �������� ������
	Ptr<ThreadFeatures> threadFeatures = features->threadFeatures();
	if (!threadFeatures)return false;
	//��������� �� �������� �������
	Ptr<RectangularPatternFeatures> rectangularPatterns = features -> rectangularPatternFeatures();
	if (!rectangularPatterns)return false;

	//��������� ���������� ��� �������� ������ (Piston)
	double radiusCylinderInternal = 1.5; //������ ���������� ���������� �������� ��� �������� ������(Piston) (cm)
	double radiusCylinderExternal = 6.5; //������ ������� ���������� �������� ��� �������� ������(Piston) (cm)
	double distanceCylinder = 5.7; //���������� ������������ �������� ������ (Piston) (cm)
	double distanceoffsetPlanes = 5.4; //���������� �������� ����� ������� ��������� ������(Piston) (cm)
	double radiusLintelInternal = 6.5; //������ ���������� ���������� ��������� ��� �������� ������(Piston) (cm)
	double radiusLintelExternal = 7.0; //������ ������� ���������� ��������� ��� �������� ������(Piston) (cm)
	double distanceLintel = -0.9; //���������� ������������ ��������� ������ (Piston) (cm)
	double quantityOne = 3; //���������� ��������� �������������� ������� ������ (Piston)
	double distanceOne = -4.2;//���������� ����� ����������� �������������� ������� ������

	// �������� ����� � ������ ���������
	Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
	if (!centerPoint) return false;
	//������ ���� �������� ������ (Piston) - �������� ������ ��������
	//�������� ������� ���������
	Ptr<Sketch> sketchCylinder = sketches->add(rootComp->xYConstructionPlane());
	if (!sketchCylinder)
		return false;
	// �������� �������� �������� ����������
	Ptr<SketchCurves> sketchCurvesCylinder = sketchCylinder->sketchCurves();
	if (!sketchCurvesCylinder)
		return false;
	//�������� ���������� ������ ����������
	Ptr<SketchCircles> sketchCircles = sketchCurvesCylinder->sketchCircles();
	if (!sketchCircles)
		return false;
	//�������� ���������� ������� ���������� ����� � ������ ��������� � ������� ����������
	sketchCircles->addByCenterRadius(centerPoint, radiusCylinderInternal);
	sketchCircles->addByCenterRadius(centerPoint, radiusCylinderExternal);
	//����� ������� ����� �������� ������ (Piston) - �������� ������ ��������

	//������ ���� �������� ������ (Piston) - ������������ ������ ��������
	//�������� ������� ��� ������������
	Ptr<Profiles> profilesCylinder = sketchCylinder->profiles();
	if (!profilesCylinder)
		return false;
	//����� ������� ��� ������������
	Ptr<Profile> profileCylinder = profilesCylinder->item(1);
	if (!profileCylinder)
		return false;
	//������������ � ��������� �������� ������ ����
	Ptr<ExtrudeFeatureInput> extrudeCylinder = extrudes->createInput(profileCylinder,
		NewBodyFeatureOperation);
	if (!extrudeCylinder)
		return false;
	//��������� ����������� ������������ � ����������
	extrudeCylinder->setDistanceExtent(false, ValueInput::createByReal(distanceCylinder));
	//����� �������� ����
	extrudeCylinder->isSolid(true);
	//�������� ������������ ������������ ������ ��������
	Ptr<ExtrudeFeature> ext = extrudes->add(extrudeCylinder);
	if (!ext) return false;
	//����� ������� ����� �������� ������ (Piston) - ������������ ������ ��������

	//������ ���� �������� ������ (Piston) - �������� �������������� ���������
	//��������� ����� ������� ���������
	Ptr<ConstructionPlanes> ctorPlanes = rootComp->constructionPlanes();
	if (!ctorPlanes) return false;
	// �������� ��������������� ���������
	Ptr<ConstructionPlaneInput> ctorPlaneInput = ctorPlanes->createInput();
	if (!ctorPlaneInput) return false;
	//���������� �������� ����� ������� ���������
	Ptr<ValueInput> offsetPlanes = ValueInput::createByReal(distanceoffsetPlanes);
	if (!offsetPlanes) return false;
	//��������� ��������� � ���������� � ��������
	bool retPlane = ctorPlaneInput->setByOffset(rootComp->xYConstructionPlane(),
		offsetPlanes);
	if (!retPlane) return false;
	//������� ����� ������� ���������
	Ptr<ConstructionPlane> ctorPlane = ctorPlanes->add(ctorPlaneInput);
	if (!ctorPlane) return false;

	//��������� ���� �������� ������ (Piston) - �������� ������ ���������
	//����� ������� ���������
	Ptr<Sketch> sketchLintel = sketches->add(ctorPlane);
	if (!sketchLintel) return false;
	// �������� �������� �������� ����
	Ptr<SketchCurves> sketchCurvesLintel = sketchLintel->sketchCurves();
	if (!sketchCurvesLintel) return false;
	//�������� ���������� ������ ���������� ��� ���������
	Ptr<SketchCircles> sketchCirclesLintel = sketchCurvesLintel->sketchCircles();
	if (!sketchCirclesLintel) return false;
	//������� ���������� ��� ���������, ������� ����� � ������ ��������� � ������� ����������
	sketchCirclesLintel->addByCenterRadius(centerPoint, radiusLintelInternal);
	sketchCirclesLintel->addByCenterRadius(centerPoint, radiusLintelExternal);
	//����� ���������� ����� �������� ������ (Piston) - �������� ������ ���������

	//����� ���� �������� ������ (Piston) - ������������ ������ ���������
	//�������� ������� ��� ������������
	Ptr<Profiles> profilesLintel = sketchLintel->profiles();
	if (!profilesLintel) return false;
	//����� ������� ��� ������������ ���������
	Ptr<Profile> profileLintel = profilesLintel->item(1);
	if (!profileLintel) return false;
	//������������ � ��������� �������������
	Ptr<ExtrudeFeatureInput> extFeatureLintel = extrudes->createInput(profileLintel,
		FeatureOperations::JoinFeatureOperation);
	//��������� ����������� ������������ � ����������
	extFeatureLintel->setDistanceExtent(false, ValueInput::createByReal(distanceLintel));
	//����� �������� ����
	extFeatureLintel->isSolid(true);
	//�������� ������������ ���������
	Ptr<ExtrudeFeature> extLintel = extrudes->add(extFeatureLintel);
	if (!extLintel) return false;
	// ����� ������ ����� �������� ������ (Piston) - ������������ ������ ���������

	Ptr<BRepBodies> bodies = rootComp->bRepBodies();
	if (!bodies)
		return false;
	//����� ���� ��� �������� �������������� �������
	Ptr<BRepBody> body = bodies->item(0);
	if (!body)
		return false;
	//�������� ����� ��� �������������� �������
	Ptr<BRepFaces> faces = body->faces();
	if (!faces)
		return false;
	// �������� ������� �������� ��� �������������� �������
	Ptr<ObjectCollection> faceCollection = ObjectCollection::create();
	if (!faceCollection)
		return false;
	//����� ������ ��� �������������� �������
	faceCollection->add(faces->item(0));
	faceCollection->add(faces->item(1));
	faceCollection->add(faces->item(2));
	//������� �������������� ��� Z ��� �������������� �������
	Ptr<ConstructionAxis> zAxis = rootComp->zConstructionAxis();
	if (!zAxis)
		return false;
	//������� ������������� ������ ��� ���������
	Ptr<RectangularPatternFeatureInput> rectangularPatternInput = rectangularPatterns->createInput(faceCollection, zAxis,
	ValueInput::createByReal(quantityOne), ValueInput::createByReal(distanceOne),
	PatternDistanceType::ExtentPatternDistanceType);
	if (!rectangularPatternInput)
		return false;
	// �������� �������������� �������
	Ptr<RectangularPatternFeature> rectangularFeature = rectangularPatterns-> add(rectangularPatternInput);
	if (!rectangularFeature)
		return false;


	Ptr<DataHub> hub = app->data()->activeHub();
	if (!hub) return false;
	Ptr<DataProjects> projects = hub->dataProjects();
	if (!projects) return false;
	Ptr<DataProject> project = nullptr;
	for (Ptr<DataProject>& proj : projects) {

		if (proj->name() == "Hydraulic Cylinder") 
		{
			project = proj;
			break;
		}
	}

	if (!project) return false;
	Ptr<DataFolder> folder = project->rootFolder();
	if (!folder) return false;

	if (!doc->saveAs("Piston", folder, "description", "tag")) return false;

	return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // XI_WIN
