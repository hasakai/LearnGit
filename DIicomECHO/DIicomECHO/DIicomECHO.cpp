// DIicomECHO.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//具体添加哪些lib库文件，其实还要看你用的函数和类，函数和类在那个lib库文件中，就要把这个库文件添加进来
#include <iostream>
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dimse.h"

int main()
{
	T_ASC_Network *net; // network struct, contains DICOM upper layer FSM etc.
	ASC_initializeNetwork(NET_REQUESTOR, 0, 1000 /* timeout */, &net);

	T_ASC_Parameters *params; // parameters of association request
	ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);

	// set calling and called AE titles
	ASC_setAPTitles(params, "ECHOSCU", "DCM4CHEE", NULL);

	// the DICOM server accepts connections at server.nowhere.com port 104
	ASC_setPresentationAddresses(params, "localhost", "127.0.0.1:11112");

	// list of transfer syntaxes, only a single entry here
	const char* ts[] = { UID_LittleEndianImplicitTransferSyntax };

	// add presentation context to association request
	ASC_addPresentationContext(params, 1, UID_VerificationSOPClass, ts, 1);

	// request DICOM association
	T_ASC_Association *assoc;
	if (ASC_requestAssociation(net, params, &assoc).good())
	{
		if (ASC_countAcceptedPresentationContexts(params) == 1)
		{
			// the remote SCP has accepted the Verification Service Class
			DIC_US id = assoc->nextMsgID++; // generate next message ID
			DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
			DcmDataset *sd = NULL; // status detail will be stored here
			// send C-ECHO-RQ and handle response
			DIMSE_echoUser(assoc, id, DIMSE_BLOCKING, 0, &status, &sd);
			delete sd; // we don't care about status detail
		}
	}
	ASC_releaseAssociation(assoc); // release association
	ASC_destroyAssociation(&assoc); // delete assoc structure
	ASC_dropNetwork(&net); // delete net structure

	std::cout << "Hello World!\n";
}


