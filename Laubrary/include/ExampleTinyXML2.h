#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "Laubrary_Export.h"
#include "LaubraryConfig.h"
#include "NPTrackingTools.h"
#include "tinyxml2.h"

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>


namespace example{

    // Example function
    Laubrary_EXPORT void createExampleXML()
    {

        XMLDocument doc;
        doc.LoadFile( "dream.xml" );

        // Structure of the XML file:
        // - Element "PLAY"      the root Element, which is the 
        //                       FirstChildElement of the Document
        // - - Element "TITLE"   child of the root PLAY Element
        // - - - Text            child of the TITLE Element

        // Navigate to the title, using the convenience function,
        // with a dangerous lack of error checking.
        const char* title = doc.FirstChildElement( "PLAY" )->FirstChildElement( "TITLE" )->GetText();
        fprintf(stdout,  "Name of play (1): %s\n", title );

        // Text is just another Node to TinyXML-2. The more
        // general way to get to the XMLText:
        XMLText* textNode = doc.FirstChildElement( "PLAY" )->FirstChildElement( "TITLE" )->FirstChild()->ToText();
        title = textNode->Value();
        fprintf(stdout,  "Name of play (2): %s\n", title );

    };
}


#endif