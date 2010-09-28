//
// 
// Copyright (C) 2004-2007  Autodesk, Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
// 
//
#ifndef FDOFUNCTIONM_H
#define FDOFUNCTIONM_H

#include <FdoExpressionEngine.h>
#include <FdoExpressionEngineINonAggregateFunction.h>


// ============================================================================
// The class FdoFunctionM implements the Expression Engine function M.
// ============================================================================

class FdoFunctionM : public FdoExpressionEngineINonAggregateFunction
{

    public:

        // ********************************************************************
        // *                         Public Interfaces                        *
        // ********************************************************************

        // Create:
        //  Function to create an instance of this class.

		EXPRESSIONENGINE_API static FdoFunctionM *Create ();

        // CreateObject:
        //  Function to create an instance of this class.

        EXPRESSIONENGINE_API virtual FdoFunctionM *CreateObject ();

        // GetFunctionDefinition:
        //  The function returns the function definition for the function M.
        //  The definition includes the list of supported signatures for the
        //  function.

        EXPRESSIONENGINE_API virtual FdoFunctionDefinition *GetFunctionDefinition ();

        // Evaluate:
        //  The function determines the function result and returns it back to
        //  the calling routine.

        EXPRESSIONENGINE_API virtual FdoLiteralValue *Evaluate (
                                    FdoLiteralValueCollection *literal_values);


	protected:
        // FdoFunctionM:
        //  The function represents the class constructor.

        EXPRESSIONENGINE_API FdoFunctionM ();

        // ~FdoFunctionArea2D:
        //  The function represents the class destructor.

        EXPRESSIONENGINE_API ~FdoFunctionM ();

    private:

        // ********************************************************************
        // *                        Private Interfaces                        *
        // ********************************************************************

        // CreateFunctionDefinition:
        //  The routine creates the function definition for the function M.

        void CreateFunctionDefinition ();

        // Dispose:
        //  The function disposes the current object.

        virtual void Dispose () { delete this; };

        // Validate:
        //  The function validates the provided parameters for the function M

        void Validate (FdoLiteralValueCollection *literal_values);


        // ********************************************************************
        // *                      Private Member Variables                    *
        // ********************************************************************

        // function_definition:
        //  The variable references the function definition for the function M
        
        FdoFunctionDefinition *function_definition;

        // is_validated:
        //  For performance reasons the arguments passed to the procedure
        //  processing the request is done once only for the time of its
        //  execution. This variable stores whether or not the validation
        //  has been performed.

        FdoBoolean is_validated;

		
        FdoPtr<FdoDoubleValue> return_double_value;


};  //  class FdoFunctionM

#endif
