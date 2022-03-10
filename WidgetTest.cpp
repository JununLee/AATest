

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkCommand.h>
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>

#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkAngleWidget.h>
#include <vtkProperty2D.h>
#include <vtkLeaderActor2D.h>
#include <vtkAngleRepresentation2D.h>
#include <vtkBiDimensionalWidget.h>
#include <vtkBiDimensionalRepresentation2D.h>
#include <vtkLineWidget.h>
#include <vtkLineRepresentation.h>
#include <vtkHandleWidget.h>
#include <vtkDistanceRepresentation3D.h>
#include "keyinDistanceRepresentation.h"
#include <vtkAngleRepresentation3D.h>
#include <vtkCamera.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkImageMapper.h>
#include "keyinAngleRepresentation.h"
#include <vtkAxisActor2D.h>
#include "keyinAngleWidget.h"
#include "myTestRepresentation.h"
#include <vtkImageTracerWidget.h>
#include <vtkContourWidget.h>
#include "keyinContourRepresentation.h"
#include "keyinContourWidget.h"
#include "keyinGlyphContourRepresentation.h"
#include <vtkProperty.h>
#include "keyinVolumeWidget.h"
#include "keyinVolumeRepresentation.h"
#include <vtkDICOMImageReader.h>
#include <vtkMarchingCubes.h>
#include <vtkMatrix4x4.h>
#include "keyinLinearMeasure.h"
#include "vtkImageActorPointPlacerLTY.h"
#include <vtkImageActorPointPlacer.h>
#include "keyinAngleMeasure.h"
#include "keyinContour.h"
#include <StaticControl/Controler/include/ImageControl.h>
#include <vtkTransform.h>
#include <vtkCylinderSource.h>
#include <vtkAssembly.h>
#include <vtkOBJExporter.h>
#include <vtkLine.h>
#include <vtkCellArray.h>


void print1(keyinContourRepresentation* rep) {

	double node[2];
	rep->GetNthNodeDisplayPosition(0, node);
	
	std::cout << node[0] << " " << node[1] << std::endl;
}
void print2() {
	std::cout << 1  << " " << 2 << std::endl;
}

class vtkBiDimensionalCallback : public vtkCommand
{
public:
	static	vtkBiDimensionalCallback*  New()
	{
		return new vtkBiDimensionalCallback;
	}

	virtual void Execute(vtkObject* caller, unsigned long, void*)
	{
		vtkBiDimensionalWidget* biDimensionalWidget =
			reinterpret_cast<vtkBiDimensionalWidget*> (caller);
		myTestRepresentation* representation =
			static_cast<myTestRepresentation*> (biDimensionalWidget->GetRepresentation());
		double p1[3];
		representation->GetPoint1DisplayPosition(p1);
		double p2[3];
		representation->GetPoint1DisplayPosition(p2);
		double p3[3];
		representation->GetPoint1DisplayPosition(p3);
		double p4[3];
		representation->GetPoint1DisplayPosition(p4);
		//显示其中一个点的屏幕坐标(px)
		std::cout << "P1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
		representation->TextMapper->SetInput("my god");
	}
	vtkBiDimensionalCallback() { }
};
int main()
{
	int WidgetType = 3;
	std::cout << "Please select the Measurement Distance WidgetType: " << std::endl;
	std::cin >> WidgetType;

	vtkSmartPointer<vtkJPEGReader> reader = vtkSmartPointer<vtkJPEGReader>::New();
	reader->SetFileName("C:\\Users\\PDC-48\\Desktop\\JununLee.jpg");
	reader->Update();
	
	vtkSmartPointer<vtkImageMapper> m = vtkSmartPointer<vtkImageMapper>::New();
	m->SetInputData(reader->GetOutput());
	m->SetColorWindow(256);
	m->SetColorLevel(128);

	vtkSmartPointer<vtkActor2D> ac = vtkSmartPointer<vtkActor2D>::New();
	ac->SetMapper(m);

	
	vtkSmartPointer<vtkImageActor> imgActor = vtkSmartPointer<vtkImageActor>::New();
	imgActor->SetInputData(reader->GetOutput());
	
	imgActor->SetPosition(0, 0, 1);

	vtkSmartPointer<vtkRenderer> render = vtkSmartPointer<vtkRenderer>::New();
	render->AddActor(imgActor);
	//render->AddActor2D(ac);
	render->SetBackground(0, 0, 0);
	render->ResetCamera();

	//render->GetActiveCamera()->SetPosition(0, 0, 1);
	//render->GetActiveCamera()->SetFocalPoint(0, 0, -1);
	//render->GetActiveCamera()->SetPosition(0, 1600, 0);
	//render->GetActiveCamera()->SetViewUp(0, 0, 1);
	//render->GetActiveCamera()->SetFocalPoint(0, 0, 0);

	vtkSmartPointer<vtkRenderWindow> rw = vtkSmartPointer<vtkRenderWindow>::New();
	rw->AddRenderer(render);
	rw->SetWindowName("MeasurementDistanceApp");
	rw->SetSize(800, 800);
	rw->Render();

	vtkSmartPointer<vtkRenderWindowInteractor> rwi =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	rwi->SetRenderWindow(rw);

	//vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

	rwi->SetInteractorStyle(style);

	if (WidgetType == 8) {
		Sctl::ImageControl ctl;
		//ctl.setFilePath("G:\\test\\DCM_liaoximin");
		//ctl.setFilePath("G:\\test\\LI JIN HAI\\38616\\MR Sag T1 cube+C _7");
		//ctl.setFilePath("G:\\test\\TestDirection\\x90\\ScalarVolume_20");
		//ctl.setFilePath("G:\\test\\TestDirection\\y90\\ScalarVolume_42");
		//ctl.setFilePath("G:\\test\\TestDirection\\ScalarVolume_20");
		ctl.setFilePath("G:\\test\\TestDirection\\ScalarVolume_49");
		//ctl.setFilePath("G:\\test\\TestDirection\\ScalarVolume_68");
		ctl.UpdateImage();
		Sird::DicomBaseObject dicominfo = ctl.getDicomInfo();
		ctl.AddMarker(render,rwi,0,0,0.2,0.2);

		std::vector<double> rotation = dicominfo.imageOrientation;
		double a[3] = { rotation[0],rotation[1],rotation[2] };
		double b[3] = { rotation[3],rotation[4],rotation[5] };
		double c[3];
		vtkMath::Cross(a, b, c);

		vtkMatrix4x4* vtkmat = vtkMatrix4x4::New();
		vtkmat->Identity();
		vtkmat->SetElement(0, 0, rotation[0]);
		vtkmat->SetElement(1, 0, rotation[1]);
		vtkmat->SetElement(2, 0, rotation[2]);

		vtkmat->SetElement(0, 1, rotation[3]);
		vtkmat->SetElement(1, 1, rotation[4]);
		vtkmat->SetElement(2, 1, rotation[5]);

		vtkmat->SetElement(0, 2, c[0]);
		vtkmat->SetElement(1, 2, c[1]);
		vtkmat->SetElement(2, 2, c[2]);

	

		vtkImageData* input = ctl.GetVtkImageData();

		vtkSmartPointer<vtkMarchingCubes> marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
		marchingCubes->SetInputData(input);
		marchingCubes->SetValue(0, 200);
		marchingCubes->ComputeGradientsOn();
		marchingCubes->ComputeNormalsOn();
		marchingCubes->ComputeScalarsOff();

		vtkSmartPointer<vtkPolyDataMapper> manpper = vtkSmartPointer<vtkPolyDataMapper>::New();
		manpper->SetInputConnection(marchingCubes->GetOutputPort());

		vtkActor *actor = vtkActor::New();
		actor->SetMapper(manpper);

		//vtkmat->SetElement(0, 3, input->GetOrigin()[0]);
		//vtkmat->SetElement(1, 3, input->GetOrigin()[1]);
		//vtkmat->SetElement(2, 3, input->GetOrigin()[2]);


		vtkMatrix4x4* vtkmat1 = vtkMatrix4x4::New();
		vtkmat1->Identity();
		vtkmat1->SetElement(0, 0, -1);
		vtkmat1->SetElement(1, 1, -1);

		vtkMatrix4x4::Multiply4x4(vtkmat, vtkmat1, vtkmat);
		vtkMatrix4x4::Multiply4x4(vtkmat1, vtkmat, vtkmat);

		vtkSmartPointer<vtkAssembly> assemb = vtkSmartPointer<vtkAssembly>::New();
		assemb->AddPart(actor);
		vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
		axes->SetTotalLength(350, 350, 350);
		vtkSmartPointer<vtkAxesActor> axes1 = vtkSmartPointer<vtkAxesActor>::New();
		axes1->SetTotalLength(350, 350, 350);
		//axes->SetPosition(actor->GetCenter());
		axes->SetPosition(input->GetOrigin());
		assemb->AddPart(axes);


		vtkMatrix4x4* mat = actor->GetMatrix();
		
		assemb->SetUserMatrix(vtkmat);
		
		double* angle = actor->GetOrientation();

		render->RemoveActor(imgActor);
		render->AddActor(assemb);
		render->AddActor(axes1);

		rw->Render();
		rwi->Initialize();

		rwi->Start();
	}
	
	if (WidgetType == 4) {

		vtkSmartPointer<vtkDICOMImageReader> volumeReader = vtkSmartPointer<vtkDICOMImageReader>::New();
		volumeReader->SetDirectoryName("E:\\Data\\lxm");
		//volumeReader->SetDirectoryName("E:\\Data\\model20200113\\CT");
		//volumeReader->SetDirectoryName("E:\\Data\\CBCT039");
		//volumeReader->SetDirectoryName("E:\\ScalarVolume_24");
		volumeReader->Update();
		vtkImageData* input = volumeReader->GetOutput();

		vtkSmartPointer<vtkMarchingCubes> marchingCubes = vtkSmartPointer<vtkMarchingCubes>::New();
		marchingCubes->SetInputData(input);
		marchingCubes->SetValue(0, -750);
		marchingCubes->ComputeGradientsOn();
		marchingCubes->ComputeNormalsOn();
		marchingCubes->ComputeScalarsOff();
		
 
		vtkSmartPointer<vtkPolyDataMapper> manpper = vtkSmartPointer<vtkPolyDataMapper>::New();
		manpper->SetInputConnection(marchingCubes->GetOutputPort());
		
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(manpper);

		vtkMatrix4x4* mat = actor->GetMatrix();

		double volume = 0;
		int seed[3] = { 275,181,318 };
		//int seed[3] = { 233,138,66 };
		//int seed[3] = { 211,214,148 };

		render->RemoveActor(imgActor);
		render->AddActor(actor);

		rw->Render();
		//rwi->Initialize();

		//vtkSmartPointer<keyinVolumeWidget> volumeWidget = vtkSmartPointer<keyinVolumeWidget>::New();
		//volumeWidget->SetInteractor(rwi);

		//vtkSmartPointer<keyinVolumeRepresentation> volumeRep = vtkSmartPointer<keyinVolumeRepresentation>::New();

		//volumeRep->SetLabelFormat(u8"%.1fmm³");
		//volumeRep->GetLabelTextProperty()->SetFontSize(15);
		////volumeRep->GetLabelTextProperty()->SetFontFamily(VTK_FONT_FILE);
		////volumeRep->GetLabelTextProperty()->SetFontFile("E://SVN//spine-op//program//project//SpineKNS//Assets//AlibabaFonts//Alibaba-PuHuiTi-Regular.ttf");
		//volumeRep->GetLabelTextProperty()->SetColor(0, 0, 0);
		//volumeRep->GetLabelTextProperty()->SetJustificationToCentered();
		//volumeRep->GetLabelTextProperty()->SetVerticalJustificationToCentered();
		//volumeRep->SetTextBackImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\myactor.png");
		//volumeWidget->SetRepresentation(volumeRep);
		//volumeWidget->On();

		//volumeRep->volumeCalculate(input, seed, 0, 200);
		//volumeRep->GetVolumeProperty()->SetColor(0, 1, 1);

		////render->ResetCamera();

		//rwi->Start();

		//模型输出obj
		vtkSmartPointer<vtkOBJExporter> porter = vtkSmartPointer<vtkOBJExporter>::New();
		porter->SetFilePrefix("E:\\model");
		porter->SetInput(rw);
		porter->Write();

	}

	//contour
	if (WidgetType == 7) {
		keyinContour *contour = new keyinContour;
		contour->SetLineColor(0, 1, 1);
		contour->SetCompleteCallback(print1);
		contour->contourWidget->startCallback = std::bind(print2); 
		contour->contourRep->GetActiveProperty()->SetColor(1, 0, 0);
		//contour->contourRep->GetActiveActor()->SetScale(0.5);
		contour->contourRep->SetHandleSize(0.01);
		std::cout <<"size"<< contour->contourRep->GetHandleSize() << std::endl;
		contour->On(rwi);

		

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}
	if (WidgetType == 3) {
		vtkSmartPointer<keyinContourWidget> widget = vtkSmartPointer<keyinContourWidget>::New();
		widget->SetInteractor(rwi);
		//vtkSmartPointer<myContourRepresentation> contourRep = vtkSmartPointer<myContourRepresentation>::New();
		widget->SetFollowCursor(1);
		keyinGlyphContourRepresentation* rep = reinterpret_cast<keyinGlyphContourRepresentation*>(widget->GetContourRepresentation());
		rep->GetLinesProperty()->SetColor(0, 1, 1);
		//rep->GetLinesProperty()->SetLineWidth(3);
		//rep->GetProperty()->SetColor(1, 0, 0);
		//rep->GetProperty()->SetPointSize(30);
		//rep->SetPixelTolerance(10);
		//rep->GetPointPlacer()->get
		//float a = rep->GetProperty()->GetPointSize();
		//rep->GetActiveProperty()->SetColor(1, 0, 0);

		//rep->GetPointPlacer()->SetPixelTolerance(6);
		//widget->SetContinuousDraw(1);
		widget->On();

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}

	//distance
	if (WidgetType == 5) {

		vtkSmartPointer <vtkImageActorPointPlacer> pointPlacer = vtkImageActorPointPlacer::New();
		imgActor->SetDisplayExtent(imgActor->GetInput()->GetExtent());
		pointPlacer->SetImageActor(imgActor);

		keyinLinearMeasure* linearMeasure = new keyinLinearMeasure;
		linearMeasure->linearRep->GetPoint1Representation()->SetPointPlacer(pointPlacer);
		linearMeasure->linearRep->GetPoint2Representation()->SetPointPlacer(pointPlacer);
		linearMeasure->SetLineColor(0, 1, 1);
		linearMeasure->SetLineWidth(3);
		linearMeasure->SetFontSize(20);
		linearMeasure->SetTextColor(0, 0, 0);
		linearMeasure->SetFontFile("E://SVN//spine-op//program//project//SpineKNS//Assets//AlibabaFonts//Alibaba-PuHuiTi-Regular.ttf");
		linearMeasure->SetTextBackImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\myactor.png");
		linearMeasure->SetPointImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\point.png");
		linearMeasure->SetHandleImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\control.png");
		linearMeasure->On(rwi);
		 

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}
	if (WidgetType == 0)
	{
		//实例化Widget
		vtkSmartPointer<vtkDistanceWidget> distanceWidget = vtkSmartPointer<vtkDistanceWidget>::New();
		//指定渲染窗口交互器,来监听用户事件
		distanceWidget->SetInteractor(rwi);
		//必要时使用观察者/命令模式创建回调函数(此处没用)
		//创建几何表达实体。用SetRepresentation()把事件与Widget关联起来
		//或者使用Widget默认的几何表达实体

		//vtkSmartPointer<vtkDistanceRepresentation3D> distanceRep = vtkSmartPointer<vtkDistanceRepresentation3D>::New();
		vtkSmartPointer<keyinDistanceRepresentation> distanceRep = vtkSmartPointer<keyinDistanceRepresentation>::New();
		distanceRep->GetAxisProperty()->SetColor(0, 1, 1);
		distanceRep->GetAxisProperty()->SetLineWidth(3);

		distanceRep->SetLabelFormat("%.1fmm");
		distanceRep->GetAxis()->GetTitleTextProperty()->SetFontSize(20);
		distanceRep->GetAxis()->GetTitleTextProperty()->SetFontFamily(VTK_FONT_FILE);
		distanceRep->GetAxis()->GetTitleTextProperty()->SetFontFile("E://SVN//spine-op//program//project//SpineKNS//Assets//AlibabaFonts//Alibaba-PuHuiTi-Regular.ttf");
		distanceRep->GetAxis()->GetTitleTextProperty()->SetColor(0, 0, 0);
		distanceRep->GetAxis()->GetTitleTextProperty()->SetJustificationToCentered();
		distanceRep->GetAxis()->GetTitleTextProperty()->SetVerticalJustificationToCentered();

		distanceWidget->SetRepresentation(distanceRep);

		//distanceWidget->CreateDefaultRepresentation();
		//vtkDistanceRepresentation* representation = static_cast<vtkDistanceRepresentation*> (distanceWidget->GetRepresentation());
		//representation->SetLabelFormat("%-#6.3g mm");
		//representation->SetTolerance(10);
		//representation->SetRulerDistance(0);
		//representation->SetNumberOfRulerTicks(0);

		//激活Widget
		distanceWidget->On();

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}
	//vtkAngleWidget

	if (WidgetType == 6) {
		keyinAngleMeasure* angelMeasure = new keyinAngleMeasure;
		angelMeasure->SetLineColor(0, 1, 1);
		angelMeasure->SetLineWidth(3);
		angelMeasure->SetFontSize(20);
		angelMeasure->SetTextColor(0, 0, 0);
		angelMeasure->SetLabelFormat(u8"%.1f°");
		angelMeasure->SetFontFile("E://SVN//spine-op//program//project//SpineKNS//Assets//AlibabaFonts//Alibaba-PuHuiTi-Regular.ttf");
		angelMeasure->SetTextBackImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\myactor.png");
		angelMeasure->SetPointImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\point.png");
		angelMeasure->SetHandleImageFile("C:\\Users\\PDC-48\\Desktop\\AATest\\Image\\control.png");
		angelMeasure->On(rwi);

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}
	if (WidgetType == 1)
	{
		vtkSmartPointer<keyinAngleWidget> angleWiget = vtkSmartPointer<keyinAngleWidget>::New();
		angleWiget->SetInteractor(rwi);
		//创建个性化的实体图标
		//vtkSmartPointer<vtkAngleRepresentation2D> angleRep = vtkSmartPointer<vtkAngleRepresentation2D>::New();
		vtkSmartPointer< keyinAngleRepresentation> angleRep = vtkSmartPointer < keyinAngleRepresentation>::New();
		angleRep->GetRay1()->GetProperty()->SetColor(0, 1, 1);
		angleRep->GetRay1()->GetProperty()->SetLineWidth(3);
		angleRep->GetRay2()->GetProperty()->SetColor(0, 1, 1);
		angleRep->GetRay2()->GetProperty()->SetLineWidth(3);
		angleRep->GetArc()->GetProperty()->SetColor(0, 1, 1);
		angleRep->GetArc()->GetProperty()->SetLineWidth(3);
		angleRep->SetLabelFormat(u8"%.1f°");
		angleRep->GetArc()->GetLabelTextProperty()->SetFontSize(20);
		angleRep->GetArc()->GetLabelTextProperty()->SetFontFamily(VTK_FONT_FILE);
		angleRep->GetArc()->GetLabelTextProperty()->SetFontFile("E://SVN//spine-op//program//project//SpineKNS//Assets//AlibabaFonts//Alibaba-PuHuiTi-Regular.ttf");
		angleRep->GetArc()->GetLabelTextProperty()->SetColor(0, 0, 0);
		angleWiget->SetRepresentation(angleRep);
		angleWiget->On();

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}


	//vtkBiDimensionalWidget
	if (WidgetType == 2)
	{
		vtkSmartPointer<vtkBiDimensionalWidget> bidimensionalWidget = vtkSmartPointer<vtkBiDimensionalWidget>::New();
		bidimensionalWidget->SetInteractor(rwi);
		//采用默认的图标
		vtkSmartPointer< myTestRepresentation> angleRep = vtkSmartPointer < myTestRepresentation>::New();
		bidimensionalWidget->SetRepresentation(angleRep);
		//添加“观察者-命令模式（命令子类方案）”
		vtkSmartPointer<vtkBiDimensionalCallback> bidiCallback =
			vtkSmartPointer<vtkBiDimensionalCallback>::New();
		bidimensionalWidget->AddObserver(vtkCommand::InteractionEvent, bidiCallback);
		bidimensionalWidget->On();

		rw->Render();
		rwi->Initialize();
		rwi->Start();
	}
	return 0;
}