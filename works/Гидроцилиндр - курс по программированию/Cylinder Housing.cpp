
#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>

using namespace adsk::core;
using namespace adsk::fusion;

Ptr<Application> app;
Ptr<UserInterface> ui;

Ptr<ValueCommandInput> _strHoleDiametr;
Ptr<ValueCommandInput> _strDiametr;
Ptr<ValueCommandInput> _strDistance;
Ptr<ImageCommandInput> _img;

class OnInputChangedEventHander : public adsk::core::InputChangedEventHandler
{
public:
    void notify(const Ptr<InputChangedEventArgs>& eventArgs) override;

};
// ���������� ������� CommandExecute
class CommandExecuteEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override;

};

//���������� ������� CommandDestroyed
class OnDestroyEventHandler : public adsk::core::CommandEventHandler
{
public:
    void notify(const Ptr<CommandEventArgs>& eventArgs) override;
};


// ���������� ������� CommandCreated
class CommandCreatedFinderEventHandler : public adsk::core::CommandCreatedEventHandler
{
public:
    void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override;
private:
    OnDestroyEventHandler onDestroyHandler;
    CommandExecuteEventHandler gearCommandExecute;
} _cmdCreatedHandler;

extern "C" XI_EXPORT bool run(const char* context)
{
    app = Application::get();
    if (!app)
        return false;

    ui = app->userInterface();
    if (!ui)
        return false;

    //�������� �������, ������� ���� �������.
    Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
    if (!commandDefinitions)
        return nullptr;

    // �������� ������������  ������� ��� ������� ���, ���� �� ��� �� ����������.
    Ptr<CommandDefinition> cmdDef = commandDefinitions->itemById("cmdDetails");
    if (!cmdDef)
    {
        cmdDef = commandDefinitions->addButtonDefinition("cmdDetails",
            "Cylinder",
            "Create details.");
    }

    //������������ � ������� ���������� �������
    Ptr<CommandCreatedEvent> commandCreatedEvent = cmdDef->commandCreated();
    if (!commandCreatedEvent)
        return false;

    commandCreatedEvent->add(&_cmdCreatedHandler);

    //��������� ����������� �������.
    cmdDef->execute();

    //������������� ���������� ������ ����� ������ ��� ��������
    adsk::autoTerminate(false);

    return true;
}

// ���������� ������� CommandCreated
void CommandCreatedFinderEventHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
    if (eventArgs)
    {
        //  �������� �������, ������� ���� �������
        Ptr<Command> command = eventArgs->command();
        if (command)
        {


            // ������������ � ������� ���������� �������.
            Ptr<CommandEvent> onDestroy = command->destroy();
            if (!onDestroy)
                return;
            bool isOk = onDestroy->add(&onDestroyHandler);
            if (!isOk)
                return;

            // ������������ � ����� ����������� �������
            Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
            if (!onInputChanged)
                return;



            //  �������� ��������� CommandInputs, ��������� � ��������.
            Ptr<CommandInputs> inputs = command->commandInputs();
            if (!inputs)
                return;


            Ptr<CommandEvent> executeEvent = command->execute();
            if (!executeEvent)
                return;
            isOk = executeEvent->add(&gearCommandExecute);
            if (!isOk)
                return;
            //���������� ����������
            double holeDiam = _strHoleDiametr; // ������ ���������� ���������� ��� �������� ������ (Cylinder Housing) (cm)
            double Diam = _strDiametr;  // ������ ������� ���������� ��� �������� ������ (Cylinder Housing) (cm)
            double Distance = _strDistance;  // ���������� ������������ ��� �������� ������ (Cylinder Housing) (cm)


            //�������� ����������� � �������
            _img = inputs->addImageCommandInput("Cylinder", "", "C:/Users/User/AppData/Roaming/Autodesk/Autodesk Fusion 360/API/Scripts/Cylinder Housing/Cylinder.png");
            if (!_img)
                return;

            //������� ��������� ���� ������ ��� ������.
            Ptr<TextBoxCommandInput> text = inputs->addTextBoxCommandInput("textBox", "", "Enter an outside diameter greater than 0.", 2, true);
            if (!text)
                return;

            //�������� �����  ��������
            _strDiametr = inputs->addValueInput("Diam", "Diameter (mm)", "mm", ValueInput::createByReal(Diam));
            if (!_strDiametr)
                return;
            // ������� ��������� ���� ������ ��� ������.
            Ptr<TextBoxCommandInput> text1 = inputs->addTextBoxCommandInput("textBox", "", "Enter an inside diameter greater than 0", 2, true);
            if (!text1)
                return;
            // �������� �����  ��������
            _strHoleDiametr = inputs->addValueInput("holeDiam", "Hole Diameter (mm)", "mm", ValueInput::createByReal(holeDiam));
            if (!_strHoleDiametr)
                return;
            //������� ��������� ���� ������ ��� ������.
            Ptr<TextBoxCommandInput> text2 = inputs->addTextBoxCommandInput("textBox", "", "Indicate the extrusion distance not equal 0 ", 2, true);
            if (!text2)
                return;
            //�������� �����  ��������
            _strDistance = inputs->addValueInput("Distance", "Distance (mm)", "mm", ValueInput::createByReal(Distance));
            if (!_strDistance)
                return;



        }
    }
}

//���������� ������� CommandExecute
void CommandExecuteEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{

    //��������� �� ��������
    Ptr<Documents> docs = app->documents();
    if (!docs) return;

    Ptr<Document> doc = docs->add(DocumentTypes::FusionDesignDocumentType);
    if (!doc) return;


    // ��������� �� ����������� �� �������� ���������
    Ptr<Design> design = app->activeProduct();
    if (!design) return;
    //��������� �� ���������
    Ptr<Component> rootComp = design->rootComponent();
    if (!rootComp) return;
    //��������� �� �������
    Ptr<Features> features = rootComp->features();
    if (!features) return;
    //��������� �� �����
    Ptr<Sketches> sketches = rootComp->sketches();
    if (!sketches) return;
    // ��������� �� ������������
    Ptr<ExtrudeFeatures> extrudes = features->extrudeFeatures();
    if (!extrudes) return;


    double holeDiam = _strHoleDiametr->value();
    double Diam = _strDiametr->value();
    double Distance = _strDistance->value();
    //�������� �� ���� ����������
    if ((holeDiam <= 0) || (Diam <= 0) || (Distance == 0))
    {
        ui->messageBox("Input error");
    }
    else
    {


        Ptr<Point3D> centerPoint = Point3D::create(0, 0, 0);
        if (!centerPoint) return;

        //�������� ������
        //�������� ������� ���������
        Ptr<Sketch> sketchInternal = sketches->add(rootComp->yZConstructionPlane());
        if (!sketchInternal) return;
        // �������� �������� �������� ����������
        Ptr<SketchCurves> sketchCurvesInternal = sketchInternal->sketchCurves();
        if (!sketchCurvesInternal) return;
        //�������� ���������� ������ ����������
        Ptr<SketchCircles> sketchCirclesInternal = sketchCurvesInternal->sketchCircles();
        if (!sketchCirclesInternal) return;


        sketchCirclesInternal->addByCenterRadius(centerPoint, holeDiam / 2);
        sketchCirclesInternal->addByCenterRadius(centerPoint, Diam / 2);

        //�������� ������� ��� ������������
        Ptr<Profiles> profilesInternal = sketchInternal->profiles();
        if (!profilesInternal) return;

        //����� ������� ��� ������������ ������������� ����� ������������ � ����� �������
        Ptr<Profile> profileInternal = profilesInternal->item(1);
        if (!profileInternal) return;


        //������������ � ��������� �������� ������ ����
        Ptr<ExtrudeFeatureInput> extFeatureInternal = extrudes->createInput(profileInternal, FeatureOperations::NewBodyFeatureOperation);


        //����������� ������������
        extFeatureInternal->setDistanceExtent(true, ValueInput::createByReal(Distance));

        //����� �������� ����
        extFeatureInternal->isSolid(true);

        Ptr<ExtrudeFeature> extInternal = extrudes->add(extFeatureInternal);
        if (!extInternal) return;
        // ����� ������������  �������
        //����� �������� ������
    }
    // �������� ����� � ������ ���������


    holeDiam = _strHoleDiametr->value();
    Diam = _strDiametr->value();
    Distance = _strDistance->value();
}

void OnDestroyEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
    adsk::terminate();
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
