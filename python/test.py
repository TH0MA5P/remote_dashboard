import unittest
import udp

class TestMethods(unittest.TestCase):
  
    def setUp(self):
        self.protocol = udp.protocol()
        
    def test_sucess(self):
	writeVal = 88
	self.protocol.write(udp.DST_PORT, "TGEN", udp.FORMAT_type_int32, 1, writeVal)
	readVal = self.protocol.read(udp.DST_PORT, "TGEN", udp.FORMAT_type_int32, 1)
	self.assertEqual(writeVal, readVal)

    def test_error(self):
        self.assertEqual(self.protocol.read(udp.DST_PORT, "TGEN", udp.FORMAT_type_int32, 1), 0)



if __name__ == '__main__':
    unittest.main()
    
suite = unittest.TestLoader().loadTestsFromTestCase(TestMethods)
unittest.TextTestRunner(verbosity=2).run(suite)    
    


