#include <stdio.h>
#include <string.h>
#include <limits.h>

char inputName[100];
int row, col, matrix[100][100], s[100];
int TC[100] = {}, CS[100] = {}, supply[100][100], sumRow[100] = {};
int status[100];

void minimumCell(FILE *inFile){
	int i, j, minimum;
	for(j = 0; j < col; j++){
		minimum = 0;
		for (i = 1; i < row; i++){
			if(matrix[minimum][j] > matrix[i][j]) minimum = i;
			else if(matrix[minimum][j] == matrix[i][j]) {
				if(TC[i] > TC[minimum]) minimum = i;
			}
		}
		fscanf(inFile,"%d", &supply[minimum][j]);
	}
	
	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++)
			sumRow[i] += supply[i][j];
	}
}

int updateStatus(int *satisfied){
	//1 = Satisfied
	//2 = needSupply
	//3 = Excess Row
	int i, j, nNeedSupply = 0, nExcess = 0;
	for (i = 0; i < row; i++){
		if (sumRow[i] == s[i]) {
			if(status[i] != 1) (*satisfied)--;
			status[i] = 1;
		} else if (sumRow[i] < s[i]) {
			status[i] = 2;
			nNeedSupply = 1;
		}
		else if (sumRow[i] > s[i]) {
			status[i] = 3;
			nExcess = 1;
		}
	}
	if (!nNeedSupply || !nExcess) return 0;
	return 1;
}

void findDonatur(int *donaturRow, int *receiverRow, int *donationCol){
	int i, j, k, needSup[100] = {}, nNeedSup = 0;
	for (i = 0; i < row; i++){
		if(status[i] == 2) {	//2 = needSupply
			needSup[nNeedSup] = i;
			nNeedSup++;
		}
	}

	int temp = 0, smallestIndex = 0, localSmallest[col], localDon[col], localRec[col], flag = 0;

	//Cari local-best donation
	for(j = 0; j < col; j++){
		localSmallest[j] = INT_MAX;
		
		//mencari semua kandidat donasi
		for(i = 0; i < row; i++){
			if(status[i] == 3 && supply[i][j]){		//3 = Excess Row
				for(k = 0; k < nNeedSup; k++){
					temp = matrix[i][j] - matrix[ needSup[k] ][j];
					if (temp < 0) temp *= -1;
					
					//Find local smallest
					if(localSmallest[j] > temp) {
						localSmallest[j] = temp;
						localDon[j] = i;
						localRec[j] = needSup[k];
					}
					else if (localSmallest[j] == temp){
						if( TC[needSup[k]] > TC[ localRec[j] ] ){
							localDon[j] = i;
							localRec[j] = needSup[k];
						}
					}
				}
			}
		}
		
		//mencari global smallest dan check flag
		if(localSmallest[smallestIndex] > localSmallest[j] || smallestIndex == j) {
			smallestIndex = j;
			flag = 0;
		} else if(localSmallest[smallestIndex] == localSmallest[j]) flag = 1;
	}
	
	//donasi paling optimal
	if(!flag) {
		*donaturRow = localDon[smallestIndex];
		*receiverRow = localRec[smallestIndex];
		*donationCol = smallestIndex;
	} else {
		int donCapability1, donCapability2, maxDon1, maxDon2, CSDon, CSRec, priority1, priority2;
		for (j = smallestIndex + 1; j < col; j++){
			if(localSmallest[smallestIndex] == localSmallest[j]){
				//cari kapabilitas donasi kedua donatur
				donCapability1 = supply[ localDon[smallestIndex] ][smallestIndex];
				donCapability2 = supply[ localDon[j] ][j];
				
				//cari kapasitas donasi keduanya
				////cari prioritas satisfied 1
				CSDon = CS[ localDon[smallestIndex] ];
				CSRec = CS[ localRec[smallestIndex] ];
				if(CSDon > CSRec) 							//prioritas donatur satisfied
					priority1 = localDon[smallestIndex];
				else if (CSDon < CSRec) 					//prioritas receiver satisfied
					priority1 = localRec[smallestIndex];
				else if (localDon[smallestIndex] < localRec[smallestIndex])		//satisfied index terkecil
					priority1 = localDon[smallestIndex];
				else
					priority1 = localRec[smallestIndex];
				
				////cari kapasitas prioritas satisfied 1
				maxDon1 = s[ priority1 ] - sumRow[ priority1 ];
				if(maxDon1 < 0) maxDon1 *= -1;

				////cari prioritas satisfied 2
				CSDon = CS[ localDon[j] ];
				CSRec = CS[ localRec[j] ];
				if(CSDon > CSRec) 						//prioritas donatur satisfied
					priority2 = localDon[j];
				else if (CSDon < CSRec) 				//prioritas receiver satisfied
					priority2 = localRec[j];
				else if (localDon[j] < localRec[j])		//satisfied index terkecil
					priority2 = localDon[j];
				else
					priority2 = localRec[j];
					
				////cari kapasitas prioritas satisfied 2
				maxDon2 = s[ priority2 ] - sumRow[ priority2 ];
				if(maxDon2 < 0) maxDon1 *= -1;
				
				//tentukan jumlah donasi
				if(donCapability1 < maxDon1) maxDon1 = donCapability1;
				if(donCapability2 < maxDon2) maxDon2 = donCapability2;
				
				//bandingkan jumlah donasi
				if(maxDon1 < maxDon2) {
					*donaturRow = localDon[smallestIndex];
					*receiverRow = localRec[smallestIndex];
					*donationCol = smallestIndex;
				} else if(maxDon1 > maxDon2){
					*donaturRow = localDon[j];
					*receiverRow = localRec[j];
					*donationCol = j;
					smallestIndex = j;
				} 
				
				//jika maxDon sama
				else{
					//tentukan maxDon * cell
					int hasilKali1 = maxDon1 * matrix[ localDon[smallestIndex] ][smallestIndex];
					int hasilKali2 = maxDon1 * matrix[ localDon[j] ][j];
					if(hasilKali1 <= hasilKali2) {
						*donaturRow = localDon[smallestIndex];
						*receiverRow = localRec[smallestIndex];
						*donationCol = smallestIndex;
					}
					else{
						*donaturRow = localDon[j];
						*receiverRow = localRec[j];
						*donationCol = j;
						smallestIndex = j;
					}
					//kalo sama, ambil index terkecil
				}
			}
		}
	}

}

void donate (int *donaturRow, int *receiverRow, int *donationCol){
	//tentukan kapabilitas donatur
	int donCapability = supply[*donaturRow][*donationCol];
	printf("Kapabilitas donatur = %d\n", donCapability);
	
	//tentukan kapasitas donatur berdasarkan prioritas satisfied
	////cari prioritas satisfied
	int CSDon = CS[ *donaturRow ];
	int CSRec = CS[ *receiverRow ];
	int priority;
	if(CSDon > CSRec) 							//prioritas donatur satisfied
		priority = *donaturRow;
	else if (CSDon < CSRec) 					//prioritas receiver satisfied
		priority = *receiverRow;
	else if (*donaturRow < *receiverRow)		//satisfied index terkecil
		priority = *donaturRow;
	else
		priority = *receiverRow;
		
	////cari kapasitas donatur
	int maxDon = s[ priority ] - sumRow[ priority ];
	if(maxDon < 0) maxDon *= -1;
	printf("Kapasitas = %d\n", maxDon);
	
	//tentukan jumlah donasi
	if(donCapability < maxDon) maxDon = donCapability;
	
	//check satisfied receiver
	int maxRec = s[ *receiverRow ] - sumRow[ *receiverRow ];
	printf("Jumlah donasi		= %d\n", maxDon);
	
	//lakukan donasi
	supply[*donaturRow][*donationCol] -= maxDon;
	sumRow[*donaturRow] -= maxDon;
	supply[*receiverRow][*donationCol] += maxDon;
	sumRow[*receiverRow] += maxDon;
}

void countTCC(){
	int TCC = 0, i, j;
	for(i = 0; i < row; i++)
		for(j = 0; j < col; j++)
			TCC += (matrix[i][j]*supply[i][j]);

	printf("TCC = %d\n", TCC);
}

void check() {
	int i, j;
	char stat[][20]={"SATISFIED", "NEED SUPPLY", "EXCESS ROW"};

	for(j = 0; j < col; j++) printf("\t");
	printf("    supply\tTC\tCS\tTOTAL ALLOC\tSTATUS\n");
	for(j = 0; j < col; j++) printf("\t");
	printf("    ---------------------------------------------------------\n");
	for(i = 0; i < row; i++){
		for(j = 0; j < col; j++){
			printf("%d [%d]\t", matrix[i][j], supply[i][j]);
		}
		printf("||  %d\t\t%d\t%d\t%d\t\t%s\n\n", s[i], TC[i], CS[i], sumRow[i], stat[status[i] - 1]);
	}
	printf("\n\n\n");
}

void compute() {
	FILE *inFile  = fopen(inputName, "r"); // read only 
	FILE *outFile = fopen("output.txt", "w"); // write only
	if (inFile == NULL) {   
    	printf("Error! Could not open file\n");
    	return;
	}
	
	//# Store data from input file
	//Row column
	fscanf(inFile,"%d %d", &row, &col);
	
	//Store matriks D and S, compute TC, and set supply to 0
	int i, j;
	for (i = 0; i < row; i++){
		TC[i] = 0;
		for(j = 0; j < col; j++){
			fscanf(inFile,"%d", &matrix[i][j]);
			supply[i][j] = 0;
			TC[i] += matrix[i][j];
		}
		status[i] = 0;
		sumRow[i] = 0;
	}

	//S dan CS = TC*S
	for (i = 0; i < row; i++) {
		fscanf(inFile,"%d", &s[i]);
		CS[i] = s[i] * TC[i];
	}
	
	//D
	minimumCell(inFile);

	int satisfied = row, donaturRow, receiverRow, donationCol;
	if(!updateStatus(&satisfied)) {
		printf("Soal Salah\n");
		return;
	}
	check();
	while(satisfied){
		donaturRow = -1; receiverRow = -1;
		findDonatur(&donaturRow, &receiverRow, &donationCol);	
		donate(&donaturRow, &receiverRow, &donationCol);
		updateStatus(&satisfied);
		check();
	}
	
	countTCC();	
}

void custom() {
	printf("Name = ");
	scanf("%s", inputName);
}

main (){
	int option = 0;
	printf("What is your input-file's name?\n1 = Default (input.txt)\n2 = Custom\n3 = Exit\n");
	printf("Option = ");
	scanf("%d", &option); if(option == 3) return 0;
	do{
		if(option == 1) {strcpy(inputName, "input.txt"); compute();}
		else if (option == 2) {custom(); compute();}
		else if (option != 3) printf("Not Valid\n");
		printf("Option = ");
		scanf("%d", &option); if(option == 3);
	} while(option != 3);
	printf("Program Stop\n\n");
	return 0;
}
