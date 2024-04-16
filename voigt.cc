TH1F *h;
double xmin=0;
double xmax=8192;
int xbins=8192;//Default value
TCanvas *c1;
double xlow=0,xhigh=8192;
void mygenerate(double x, double y) {

	TF1 *f_BW = new TF1("BW","[0]/((x-[1])*(x-[1])+0.25*[2]*[2])",xmin,xmax);
	TF1 *f_Gaus = new TF1("f_Gaus","TMath::Exp(-(x*x)/(2*[0]*[0]))",xmin,xmax);
	TF1Convolution *f_voigt = new TF1Convolution("BW","f_Gaus",xmin,xmax,true);
	f_voigt->SetRange(-xmax,2*xmax);
	f_voigt->SetNofPointsFFT(10000);
	TF1 *f = new TF1("f", *f_voigt, xlow, xhigh, f_voigt->GetNpar());
	f->SetNpx(10000);
	f->SetParLimits(0,1,10000000);
	f->SetParLimits(1,xlow,xhigh);
	f->SetParLimits(2,0,10000);
	f->SetParLimits(3,1,1000);
	cout<<"Fit guess, centroid: "<<x<<"\t and amplitude: "<<y<<endl;
	f->SetParameters(y,x,1,50);

	f->SetParName(0,"BW Amplitude");
	f->SetParName(1,"Peak Centroid");
	f->SetParName(2,"Width (BW)");
	f->SetParName(3,"Gaussian Sigma");
	h->Fit("f","R");
	c1->Update();
}
void myexec() { //Taken from https://root.cern.ch/doc/master/mandelbrot_8C.html
  // get event information
  int event = gPad->GetEvent();
  int px = gPad->GetEventX();
  int py = gPad->GetEventY();
 
  // some magic to get the coordinates...
  double xd = gPad->AbsPixeltoX(px);
  double yd = gPad->AbsPixeltoY(py);
  float x = gPad->PadtoX(xd);
  float y = gPad->PadtoY(yd);
 
  static float last_x;
  static float last_y;
 
  if(event!=kKeyPress)
    {
      last_x=x;
      last_y=y;
      return;
    }
 
  switch(px){
  case 'f': // fit
    mygenerate(last_x, last_y);
    break;
  case 'l':
    cout<<"Lower bound set to "<<last_x<<endl;
    xlow = last_x;
    break;
  case 'u':
    cout<<"Upper bound set to "<<last_x<<endl;
    xhigh = last_x;
    break;
  };
}

void voigt() {
	TString fname;
	cout<<"File name:"<<endl;
	cin>>fname;
	ifstream in;
	in.open(fname);
	int line=0;
	int bin=0;
	while(in.good()) {
		TString lineread;
		in>>lineread;
		line++;
		if(line==27) {
			xbins=lineread.Atoi();
			xmax=xbins;
			h = new TH1F("h","Test input",xbins,xmin,xmax);
		}
		if(line>28) {
			h->SetBinContent(bin+1,lineread.Atoi());
			bin++;
		}
	}
	c1 = new TCanvas("c1","Voigt Fitter",0,0,800,600);
	h->Draw();
	cout<<"Click on peak and the press f to fit, press l to set lower bound, press u to set upper bound"<<endl;
	gPad->AddExec("myexec","myexec()");
}
