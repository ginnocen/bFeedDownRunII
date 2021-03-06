using namespace std;
#include "uti.h"

void trueFprompt(TString tPbPb, TString inputmcP, TString inputmcNP,
                 TString weight, TString cut,
                 TString outputfile, Float_t centmin=0, Float_t centmax=100)
{
  Bool_t isPbPb = (tPbPb=="PbPbMB"||tPbPb=="PbPb")?true:false;
  TFile* infMCP = new TFile(inputmcP);
  TTree* tMCP = (TTree*)infMCP->Get("ntDkpi");
  tMCP->AddFriend("ntHlt");
  tMCP->AddFriend("ntHi");
  tMCP->AddFriend("ntSkim");
  TFile* infMCNP = new TFile(inputmcNP);
  TTree* tMCNP = (TTree*)infMCNP->Get("ntDkpi");
  tMCNP->AddFriend("ntHlt");
  tMCNP->AddFriend("ntHi");
  tMCNP->AddFriend("ntSkim");

  TString cutP = isPbPb?Form("(%s&&Dgen==23333&&DgenBAncestorpt<=0&&hiBin>=(2*%f)&&hiBin<=(2*%f))",cut.Data(),centmin,centmax):Form("(%s&&Dgen==23333&&DgenBAncestorpt<=0)",cut.Data());
  TString cutNP = isPbPb?Form("(%s&&Dgen==23333&&DgenBAncestorpt>0&&hiBin>=(2*%f)&&hiBin<=(2*%f))",cut.Data(),centmin,centmax):Form("(%s&&Dgen==23333&&DgenBAncestorpt>0)",cut.Data());

  TH1D* hcountP = new TH1D("hcountP",";D p_{T} / GeV/c;count",nPtBins,ptbin);
  TH1D* hcountNP = new TH1D("hcountNP",";D p_{T} / GeV/c;count",nPtBins,ptbin);

  tMCP->Project("hcountP","Dpt",Form("(%s)*(%s)",weight.Data(),cutP.Data()));
  hcountP->Sumw2();
  tMCNP->Project("hcountNP","Dpt",Form("(%s)*(%s)",weight.Data(),cutNP.Data()));
  hcountNP->Sumw2();
  TH1D* hcountT = (TH1D*)hcountP->Clone("hcountT");
  hcountT->Add(hcountNP);
  TH1D* hPromptFraction = (TH1D*)hcountP->Clone("hPromptFraction");
  hPromptFraction->GetYaxis()->SetTitle("Prompt fraction");
  hPromptFraction->Divide(hcountT);
  hPromptFraction->SetMinimum(0.);
  hPromptFraction->SetMaximum(1.);
  hPromptFraction->GetXaxis()->CenterTitle();
  hPromptFraction->GetYaxis()->CenterTitle();
  hPromptFraction->SetMarkerStyle(20);
  hPromptFraction->SetMarkerSize(0.8);
  TCanvas* c = new TCanvas("c","",600,600);
  hPromptFraction->Draw("pe");
  TString outputname = isPbPb?Form("_%s_cent_%.0f_%.0f",tPbPb.Data(),centmin,centmax):Form("_%s",tPbPb.Data());
  c->SaveAs(Form("plots/ctrueFprompt%s.pdf",outputname.Data()));
  TFile* outf = new TFile(Form("%s%s.root",outputfile.Data(),outputname.Data()),"recreate");
  outf->cd();
  hPromptFraction->Write();
  outf->Close();

}

int main(int argc, char* argv[])
{
  if(argc==9)
    {
      trueFprompt(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], atof(argv[7]), atof(argv[8]));
      return 0;
    }
  else if(argc==7)
    {
      trueFprompt(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
      return 0;
    }
  else
    {
      cout<<"  ERROR: invalid number of arguments"<<endl;
      return 1;
    }
}
