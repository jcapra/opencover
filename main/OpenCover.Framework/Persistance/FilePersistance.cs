﻿//
// OpenCover - S Wilde
//
// This source code is released under the MIT License; see the accompanying license file.
//
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using OpenCover.Framework.Model;
using log4net;

namespace OpenCover.Framework.Persistance
{
    public class FilePersistance : BasePersistance
    {
        private readonly ILog _logger;

        public FilePersistance(ICommandLine commandLine, ILog logger) : base(commandLine, logger)
        {
            _logger = logger;
        }

        private string _fileName;

        public void Initialise(string fileName)
        {
            _fileName = fileName;
        }

        public override void Commit()
        {
            if (_logger.IsInfoEnabled)
            {
                _logger.Info("Committing...");
            }

            try
            {
                base.Commit();
                SaveCoverageFile();
            }
            catch (Exception ex)
            {
                Trace.WriteLine(ex.Message);
                Trace.WriteLine(ex.StackTrace);
            }
        }

        private void SaveCoverageFile()
        {
            var serializer = new XmlSerializer(typeof (CoverageSession),
                                               new[] {typeof (Module), typeof (Model.File), typeof (Class)});
            var fs = new FileStream(_fileName, FileMode.Create);
            var writer = new StreamWriter(fs, new UTF8Encoding());
            serializer.Serialize(writer, CoverageSession);
            writer.Close();
        }

    }
}
