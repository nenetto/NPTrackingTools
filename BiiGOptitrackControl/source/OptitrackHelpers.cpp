#include "OptitrackHelpers.h"


// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <math.h>

// VNL
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_matrix.h>


namespace Optitrack
{


    void OptitrackHelper::ConvertMatrix(vnl_matrix<double> &R, vnl_vector_fixed<double,3> position, vnl_quaternion<double> orientation)
    {
        fprintf(stdout, "<INFO> - [OptitrackHelper::ConvertMatrix]\n");
        double x, y, z, qx, qy, qz, qw;
        x = position[0];
        y = position[1];
        z = position[2];
        qx = orientation[0];
        qy = orientation[1];
        qz = orientation[2];
        qw = orientation[3];


        R[0][0] = - 2*qy*qy - 2*qz*qz + 1;
        R[0][0] = 2*qw*qz + 2*qx*qy;
        R[0][0] = 2*qw*qy - 2*qx*qz;
        R[0][0] = x;

        R[0][0] = 2*qx*qy - 2*qw*qz;
        R[0][0] = - 2*qx*qx - 2*qz*qz + 1;
        R[0][0] = - 2*qw*qx - 2*qy*qz;
        R[0][0] = y;

        R[0][0] = - 2*qw*qy - 2*qx*qz;
        R[0][0] = 2*qw*qx - 2*qy*qz;
        R[0][0] = - 2*qx*qx - 2*qy*qy + 1;
        R[0][0] = z;

        R[0][0] = 0.0;
        R[0][0] = 0.0;
        R[0][0] = 0.0;
        R[0][0] = 1.0;

    }

    vnl_vector_fixed<double,3> OptitrackHelper::Pivoting(OptitrackTracker::Pointer tracker, unsigned int optitrackID, unsigned int sampleNumber)
    {
		fprintf(stdout, "<INFO> - [OptitrackHelper::Pivoting]\n");
		vnl_vector_fixed<double, 3> PivotOffset;

		if (tracker->GetState() != OptitrackTracker::STATE_TRACKER_Tracking)
		{
			fprintf(stdout, "#ERROR# - [OptitrackHelper::Pivoting]: tracker is not tracking!\n");
			PivotOffset[0] = 0;
			PivotOffset[1] = 0;
			PivotOffset[2] = 0;
		}
		else
		{
			fprintf(stdout, "<INFO> - [OptitrackHelper::Pivoting]: Acquiring positions\n");
			vnl_sparse_matrix<double> A(3 * sampleNumber, 6);
			vnl_matrix<double> T_acquired(3 * sampleNumber, 4);
			vnl_vector<double> b(3 * sampleNumber, 1);
			b = 0;

			OptitrackTool::Pointer mytool = tracker->GetOptitrackTool(optitrackID);

			vcl_vector<int> cols(6);
			vcl_vector<double> vals(6);
			vnl_matrix<double> T;
			for (unsigned int q = 0; q < 3 * sampleNumber; q = q + 3)
			{
				Sleep(500);
				//fprintf(stdout, "Sample %d", (q / 3) + 1);
				//Sleep(1000);

				T = mytool->GetTransformMatrix();
				
				//TEST DAVID
				fprintf(stdout, "<test David> T(0,0)=%f T(0,1)=%f T(0,2)=%f T(0,3)=%f \n", T[0][0], T[0][1], T[0][2], T[0][3]);
				fprintf(stdout, "<test David> T(1,0)=%f T(1,1)=%f T(1,2)=%f T(1,3)=%f \n", T[1][0], T[1][1], T[1][2], T[1][3]);
				fprintf(stdout, "<test David> T(2,0)=%f T(2,1)=%f T(2,2)=%f T(2,3)=%f \n", T[2][0], T[2][1], T[2][2], T[2][3]);
				fprintf(stdout, "<test David> T(3,0)=%f T(3,1)=%f T(3,2)=%f T(3,3)=%f \n", T[3][0], T[3][1], T[3][2], T[3][3]);

				// Save matrix
				for (unsigned int i = 0; i < 3; i++) //DAVID: he cambiado i < 6 por i < 4
				{
					for (unsigned int j = 0; j < 4; j++)//DAVID: he cambiado j < 3 por j < 4
					{
						T_acquired[i + q][j] = T.get(i,j);
					}
				}

				


				for (unsigned int _c = 0; _c < 6; _c++)
				{
					cols[_c] = _c;
				}


				vals[0] = T[0][0];
				vals[1] = T[0][1];
				vals[2] = T[0][2];
				vals[3] = -1.0;
				vals[4] = 0.0;
				vals[5] = 0.0;

				A.set_row(q, cols, vals);

				vals[0] = T[1][0];
				vals[1] = T[1][1];
				vals[2] = T[1][2];
				vals[3] = 0.0;
				vals[4] = -1.0;
				vals[5] = 0.0;

				A.set_row(q + 1, cols, vals);

				vals[0] = T[2][0];
				vals[1] = T[2][1];
				vals[2] = T[2][2];
				vals[3] = 0.0;
				vals[4] = 0.0;
				vals[5] = -1.0;


				A.set_row(q + 2, cols, vals);

				b[q] = -T[0][3];
				b[q + 1] = -T[1][3];
				b[q + 2] = -T[2][3];
			}

			// Solve the Linear System by LSQR
			vnl_sparse_matrix_linear_system<double> linear_system(A, b);
			vnl_lsqr lsqr(linear_system);
			vnl_vector<double> result(6);
			lsqr.minimize(result);
			lsqr.diagnose_outcome(vcl_cerr);

			// Calculate residuals

			vnl_vector<double> residuals(3 * sampleNumber, 1);
			A.mult(result, residuals);
			residuals = residuals - b;

			// Filtering Outliers 95% 2*sd
			double sd_residualsx = 0.0;
			double sd_residualsy = 0.0;
			double sd_residualsz = 0.0;
			for (unsigned int q = 0; q < 3 * sampleNumber; q = q + 3)
			{
				residuals[q] = std::abs(residuals[q]);
				residuals[q + 1] = std::abs(residuals[q + 1]);
				residuals[q + 2] = std::abs(residuals[q + 2]);

				sd_residualsx = sd_residualsx + residuals[q] * residuals[q];
				sd_residualsy = sd_residualsy + residuals[q + 1] * residuals[q + 1];
				sd_residualsz = sd_residualsz + residuals[q + 2] * residuals[q + 2];

			}
			// Limit of 2*sd -> 95% of the sample
			sd_residualsx = 2 * std::sqrt(sd_residualsx);
			sd_residualsy = 2 * std::sqrt(sd_residualsy);
			sd_residualsz = 2 * std::sqrt(sd_residualsz);

			vnl_vector<int> valid_data(sampleNumber, 1);
			unsigned int i = 0;
			for (unsigned int q = 0; q < 3 * sampleNumber; q = q + 3)
			{
				if ((sd_residualsx >= residuals[q]) &&
					(sd_residualsy >= residuals[q + 1]) &&
					(sd_residualsz >= residuals[q + 2])) //DAVID: cambio <= por >=
				{
					// Data is valid
					valid_data[i] = 1;

				}
				else
				{
					valid_data[i] = 0;
				}

				i++;
			}

			// Recalculate the result for the filtered points
			int number_valids = valid_data.one_norm();

			vnl_sparse_matrix<double> A_2(3 * number_valids, 6);
			vnl_vector<double> b_2(3 * number_valids, 1);
			b_2 = 0;

			int q = 0;
			for (unsigned int i = 0; i < number_valids; i = i++)
			{
				if (valid_data[i] == 1)
				{
					// Data valid, include

					for (unsigned int i = 0; i < 3; i++) //David: he cambiado i<6 por i<3
					{
						for (unsigned int j = 0; j < 4; j++)
						{
							T[i][j] = T_acquired[i + q][j];//David: he cambiado i*3 por i+q
						}
					}

					for (unsigned int _c = 0; _c <= 5; _c++)
					{
						cols[_c] = _c;
					}

					vals[0] = T[0][0];
					vals[1] = T[0][1];
					vals[2] = T[0][2];
					vals[3] = -1.0;
					vals[4] = 0.0;
					vals[5] = 0.0;

					A_2.set_row(q, cols, vals);

					vals[0] = T[1][0];
					vals[1] = T[1][1];
					vals[2] = T[1][2];
					vals[3] = 0.0;
					vals[4] = -1.0;
					vals[5] = 0.0;

					A_2.set_row(q + 1, cols, vals);

					vals[0] = T[2][0];
					vals[1] = T[2][1];
					vals[2] = T[2][2];
					vals[3] = 0.0;
					vals[4] = 0.0;
					vals[5] = -1.0;


					A_2.set_row(q + 2, cols, vals);

					b_2[q] = -T[0][3];
					b_2[q + 1] = -T[1][3];
					b_2[q + 2] = -T[2][3];


					q = q + 3;
				}
				else
				{
					// Data no valid, not include
				}
			}

			// Solve the Linear System by LSQR
			vnl_sparse_matrix_linear_system<double> linear_system_2(A_2, b_2);
			vnl_lsqr lsqr_2(linear_system_2);
			vnl_vector<double> result_2(6);
			lsqr_2.minimize(result_2);
			lsqr_2.diagnose_outcome(vcl_cerr);

			PivotOffset[0] = result_2[0];
			PivotOffset[1] = result_2[1];
			PivotOffset[2] = result_2[2];

			fprintf(stdout, "<Result Offset> p1=%f p2=%f p3=%f ", result[0], result[1], result[2]);
			fprintf(stdout, "<Result Offset> p4=%f p5=%f p6=%f ", result[3], result[4], result[5]);
			fprintf(stdout, "<Result 2 Offset> p1=%f p2=%f p3=%f ", result_2[0], result_2[1], result_2[2]);
			fprintf(stdout, "<Result 2 Offset> p4=%f p5=%f p6=%f ", result_2[3], result_2[4], result_2[5]);
		}

		return PivotOffset;
    }

}