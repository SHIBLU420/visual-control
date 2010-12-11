#include "visualcontrol.h"
#include "webcamwidget.h"
#include <boost/bind.hpp>
#include <QDebug>
#include <QCloseEvent>

namespace gui
{
	VisualControl::VisualControl(QWidget *parent, Qt::WFlags flags)
		: QMainWindow(parent, flags)
	{
		setupFramework();
		setupGUI();

		setMinimumSize(QSize(640,480));

		frameCaptureThread = boost::thread(
			boost::bind(&cognition::FrameCapture::startCapturing, frameCapture));

		faceDetectorThread = boost::thread(
			boost::bind(&cognition::Detector::threadStart, faceDetector));
	}

	VisualControl::~VisualControl()
	{
	}

	void VisualControl::setupFramework()
	{
		using boost::shared_ptr;
		//videoCapture = shared_ptr<VideoCapture>( new VideoCapture(0) );
		frameCapture = shared_ptr<cognition::FrameCapture>( new cognition::FrameCapture(32) );

		//update the path of the classifier to the local path of your openCV installation!
		faceDetector = shared_ptr<cognition::FaceDetector>( 
			new cognition::FaceDetector("C:/OpenCV2.1/data/haarcascades/haarcascade_frontalface_alt.xml",
			frameCapture.get(), true, 1.16));
		frameCapture->addFrameReceiver(faceDetector.get());
	}

	void VisualControl::closeEvent(QCloseEvent *event)
	{
		//webcamController->unregister();
		faceDetector->removeController(webcamWidget);

		//qDebug() << "controller count = " << faceDetector->getControllerCount();

		frameCapture->stopCapturing();

		faceDetector->requestTreadStop();

		frameCaptureThread.interrupt();

		faceDetectorThread.interrupt();

	//	qDebug() << "Close event called, joining";
		frameCaptureThread.join();

		faceDetectorThread.join();

		//event->accept();
	
		//QMainWindow::closeEvent(event);
	}

	void VisualControl::setupGUI()
	{
		webcamWidget = new WebcamWidget(this, frameCapture.get());

		faceDetector->addController(webcamWidget); 
		
		setCentralWidget(webcamWidget);
	}
}