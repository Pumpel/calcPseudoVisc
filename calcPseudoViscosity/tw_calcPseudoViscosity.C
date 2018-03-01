/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application


\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "transformGeometricField.H"

#include "argList.H"
#include "Time.H"
#include "timeSelector.H"
#include "OSspecific.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{


    argList::noParallel();
    timeSelector::addOptions();

    #include "setRootCase.H"

    #include "createTime.H"
    #include "createMesh.H"


    timeSelector::addOptions(true, true);

    Info<< "Times found:" << runTime.times() << endl;

    instantList timeDirs = timeSelector::select0(runTime, args);

    Info<< "Times selected:" << timeDirs << endl;
    Info<< "\nEnd\n" << endl;


    forAll(timeDirs, timei)
    {
    	Info << "Time directory: " << timeDirs[timei].value() << endl;
    	runTime.setTime(timeDirs[timei], timei);
    	mesh.readUpdate();

		Info<< "Reading field tau\n" << endl;
		volSymmTensorField tau
		(
			IOobject
			(
				"tau",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
			),
			mesh
		);

		Info << "Reading the field 'shearRate'. If not present, use the"
		     << "tw_shearRate command line tool first." << endl;
		volTensorField shearRate
		(
			IOobject
			(
				"shearRate",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
			),
			mesh
		);

		Info << "Creating the field pseudoViscosity" << endl;
		volTensorField pseudoViscosity_mu
		(
			IOobject
			(
				"pseudoVisc_mu",
				runTime.timeName(),
				mesh,
				IOobject::NO_READ,
				IOobject::AUTO_WRITE
			),
			mesh,
			dimensionedTensor
			(
				"pseudoVisc_mu",
				dimensionSet(1, -1, -1, 0, 0, 0, 0),
				tensor (0,0,0,0,0,0,0,0,0)
			)
		);



		volTensorField inverseShearRate(::inv(shearRate));

		pseudoViscosity_mu = inverseShearRate & tau;

		pseudoViscosity_mu.write();
    }

    return 0;
}


// ************************************************************************* //
