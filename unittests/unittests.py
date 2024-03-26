from utils import *
from unittest import TestCase

"""
For each operation, you should write tests to test  on matrices of different sizes.
Hint: use dp_mc_matrix to generate dumbpy and numc matrices with the same data and use
      cmp_dp_nc_matrix to compare the results
"""

class TestErrors(TestCase):
    def test_add_typeerr(self):
        try:
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
            dp_mat2, nc_mat2 = 4, 4
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_sub_typeerr(self):
        try:
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
            dp_mat2, nc_mat2 = 4, 4
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_mul_typeerr(self):
        try:
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
            dp_mat2, nc_mat2 = 4, 4
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_pow_typeerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(4, 4, seed=1)
            is_correct, speed_up = compute([dp_mat, dp_mat2], [nc_mat, nc_mat2], "pow")
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_pow_typeerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
            is_correct, speed_up = compute([dp_mat, 5.0], [nc_mat, 5.0], "pow")
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_add_valueerr(self):
        try:
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(4, 4, seed=1)
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_sub_valueerr(self):
        try:
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(4, 4, seed=1)
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_mul_valueerr(self):
        try:
            dp_mat1, nc_mat1 = rand_dp_nc_matrix(4, 2, seed=0)
            dp_mat2, nc_mat2 = rand_dp_nc_matrix(4, 4, seed=1)
            is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_pow_valueerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
            is_correct, speed_up = compute([dp_mat, -3], [nc_mat, -3], "pow")
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_pow_valueerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            is_correct, speed_up = compute([dp_mat, 3], [nc_mat, 3], "pow")
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_subscript_indexerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(1, 5, seed=0)
            dp_slice = dp_mat[6]
            nc_slice = nc_mat[6]
            self.assertTrue(False)
        except IndexError as e:
            print(e)
            pass

    def test_subscript_indexerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 1, seed=0)
            dp_slice = dp_mat[6]
            nc_slice = nc_mat[6]
            self.assertTrue(False)
        except IndexError as e:
            print(e)
            pass

    def test_subscript_indexerr3(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(1, 5, seed=0)
            dp_slice = dp_mat[2][4]
            nc_slice = nc_mat[2][4]
            self.assertTrue(False)
        except IndexError as e:
            print(e)
            pass

    def test_subscript_indexerr4(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 1, seed=0)
            dp_slice = dp_mat[2][4]
            nc_slice = nc_mat[2][4]
            self.assertTrue(False)
        except IndexError as e:
            print(e)
            pass

    def test_subscript_indexerr5(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 5, seed=0)
            dp_slice = dp_mat[-2]
            nc_slice = nc_mat[-2]
            self.assertTrue(False)
        except IndexError as e:
            print(e)
            pass

    def test_subscript_typeerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 5, seed=0)
            dp_slice = dp_mat["hi"]
            nc_slice = nc_mat["hi"]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_subscript_typeerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 1, seed=0)
            dp_slice = dp_mat[2][2]
            nc_slice = nc_mat[2][2]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_subscript_valueerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 1, seed=0)
            dp_slice = dp_mata[0:4:2]
            nc_slice = nc_mata[0:4:2]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_subscript_valueerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 1, seed=0)
            dp_slice = dp_mat[0:0]
            nc_slice = nc_mat[0:0]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_subscript_valueerr3(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 5, seed=0)
            dp_slice = dp_mata[0:4:2][3]
            nc_slice = nc_mata[0:4:2][3]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_subscript_valueerr4(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(5, 5, seed=0)
            dp_slice = dp_mat[0:0][3]
            nc_slice = nc_mat[0:0][3]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_set_subscript_typeerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[2][4] = [1, 3, 5]
            nc_mat[2][4] = [1, 3, 5]
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_set_subscript_typeerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[:][4] = 5
            nc_mat[:][4] = 5
            self.assertTrue(False)
        except TypeError as e:
            print(e)
            pass

    def test_set_subscript_valueerr1(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[:][4] = [1, 2, 3, 4, 5, 6]
            nc_mat[:][4] = [1, 2, 3, 4, 5, 6]
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_set_subscript_valueerr2(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[4][:] = [1, 2, "hi"]
            nc_mat[4][:] = [1, 2, "hi"]
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_set_subscript_valueerr3(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[:][4] = [1, 2, "hi"]
            nc_mat[:][4] = [1, 2, "hi"]
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_set_subscript_valueerr4(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[:][2:4] = [[1, 2], [1, 2]]
            nc_mat[:][2:4] = [[1, 2], [1, 2]]
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_set_subscript_valueerr5(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[:][2:4] = [[1, 2], [1, 2, 3, 4], [1, 2]]
            nc_mat[:][2:4] = [[1, 2], [1, 2, 3, 4], [1, 2]]
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass

    def test_set_subscript_valueerr6(self):
        try:
            dp_mat, nc_mat = rand_dp_nc_matrix(3, 5, seed=0)
            dp_mat[:][2:4] = [[1, 2], [1, 2], [1, "hi"]]
            nc_mat[:][2:4] = [[1, 2], [1, 2], [1, "hi"]]
            self.assertTrue(False)
        except ValueError as e:
            print(e)
            pass


    

class TestAdd(TestCase):
    def test_small_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10, 10, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(10, 10, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 2000, seed=5)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 2000, seed=6)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20000, 18000, seed=3)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(20000, 18000, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_chained_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(50, 50, seed=1)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(50, 50, seed=2)
        dp_mat3, nc_mat3 = rand_dp_nc_matrix(50, 50, seed=3)
        dp_mat4, nc_mat4 = rand_dp_nc_matrix(50, 50, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2, dp_mat3, dp_mat4], 
            [nc_mat1, nc_mat2, nc_mat3, nc_mat4], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestSub(TestCase):
    def test_small_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10, 10, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(10, 10, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 2000, seed=5)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 2000, seed=6)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(20000, 18000, seed=3)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(20000, 18000, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_chained_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(50, 50, seed=1)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(50, 50, seed=2)
        dp_mat3, nc_mat3 = rand_dp_nc_matrix(50, 50, seed=3)
        dp_mat4, nc_mat4 = rand_dp_nc_matrix(50, 50, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2, dp_mat3, dp_mat4], 
            [nc_mat1, nc_mat2, nc_mat3, nc_mat4], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestAbs(TestCase):
    def test_small_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(10, 10, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 2000, seed=5)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(20000, 18000, seed=3)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestNeg(TestCase):
    def test_small_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(10, 10, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
    def test_medium_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 2000, seed=5)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(20000, 18000, seed=3)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)


class TestMul(TestCase):
    def test_small_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10, 10, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(10, 10, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(900, 720, seed=3)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(720, 1080, seed=6)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    # def test_large_mul(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat1, nc_mat1 = rand_dp_nc_matrix(5000, 4300, seed=5)
    #     dp_mat2, nc_mat2 = rand_dp_nc_matrix(4300, 3000, seed=4)
    #     is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)

    def test_chained_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(50, 50, seed=1)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(50, 50, seed=2)
        dp_mat3, nc_mat3 = rand_dp_nc_matrix(50, 50, seed=3)
        dp_mat4, nc_mat4 = rand_dp_nc_matrix(50, 50, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2, dp_mat3, dp_mat4], 
            [nc_mat1, nc_mat2, nc_mat3, nc_mat4], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestPow(TestCase):
    def test_small_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(10, 10, seed=0)
        is_correct, speed_up = compute([dp_mat, 1000], [nc_mat, 1000], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(900, 900, seed=3)
        is_correct, speed_up = compute([dp_mat, 10], [nc_mat, 10], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(5000, 5000, seed=5)
        is_correct, speed_up = compute([dp_mat, 10], [nc_mat, 10], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    # def test_XL_pow(self):
    #     # TODO: YOUR CODE HERE
    #     dp_mat, nc_mat = rand_dp_nc_matrix(5000, 5000, seed=9)
    #     is_correct, speed_up = compute([dp_mat, 1000], [nc_mat, 1000], "pow")
    #     self.assertTrue(is_correct)
    #     print_speedup(speed_up)

    def test_zero_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=8)
        is_correct, speed_up = compute([dp_mat, 0], [nc_mat, 0], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_one_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=7)
        is_correct, speed_up = compute([dp_mat, 1], [nc_mat, 1], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestGet(TestCase):
    def test_get(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places))

class TestSet(TestCase):
    def test_set(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEquals(round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places))

class TestShape(TestCase):
    def test_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)

class TestSubscript(TestCase):
    def test_subscript_basic(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(69, 34, seed=0)
        dp_mat_int_key = dp_mat[68]
        nc_mat_int_key = nc_mat[68]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_int_key, nc_mat_int_key))
        dp_mat_slice_key = dp_mat[0:32]
        nc_mat_slice_key = nc_mat[0:32]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_slice_key, nc_mat_slice_key))
        dp_mat_slice_slice_key = dp_mat[0:34][0:32]
        nc_mat_slice_slice_key = nc_mat[0:34][0:32]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_slice_slice_key, nc_mat_slice_slice_key))
        dp_mat_slice_int_key = dp_mat[0:34][17]
        nc_mat_slice_int_key = nc_mat[0:34][17]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_slice_int_key, nc_mat_slice_int_key))
        dp_mat_int_slice_key = dp_mat[68][4:30]
        nc_mat_int_slice_key = nc_mat[68][4:30]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_int_slice_key, nc_mat_int_slice_key))
        dp_mat_int_int_key = dp_mat[68][0]
        nc_mat_int_int_key = nc_mat[68][0]
        self.assertTrue(dp_mat_int_int_key == nc_mat_int_int_key)

    def test_slice_parents(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(120, 69, seed=0)
        dp_mat_slice1 = dp_mat[119]
        nc_mat_slice1 = nc_mat[119]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_slice1, nc_mat_slice1))
        dp_mat_slice2 = dp_mat[119]
        nc_mat_slice2 = nc_mat[119]
        dp_mat_slice1 = None
        nc_mat_slice1 = None
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_slice2, nc_mat_slice2))

    def test_nested_slice(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(120, 69, seed=0)
        dp_mat_child = dp_mat[119]
        nc_mat_child = nc_mat[119]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_child, nc_mat_child))
        dp_mat_grandchild = dp_mat[119]
        nc_mat_grandchild = nc_mat[119]
        dp_mat_child = None
        nc_mat_child = None
        self.assertTrue(cmp_dp_nc_matrix(dp_mat_grandchild, nc_mat_grandchild))

class TestSetSubscript(TestCase):
    def test_set_subscript_basic(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(5, 5, seed=0)
        dp_mat[4] = [1, 3, 5, 10, 25]
        nc_mat[4] = [1, 3, 5, 10, 25]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[3:4] = [1, 3, 5, 10, 25]
        nc_mat[3:4] = [1, 3, 5, 10, 25]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[0:1, 0:1] = 50.5
        nc_mat[0:1, 0:1] = 50.5
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[1, :] = [2, 2, 2, 2, 2]
        nc_mat[1, :] = [2, 2, 2, 2, 2]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[2:4, 3] = [4, 4]
        nc_mat[2:4, 3] = [4, 4]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[1:3, 0:2] = [[0, 1], [2, 3]]
        nc_mat[1:3, 0:2] = [[0, 1], [2, 3]]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[1:-2, 0:-3] = [[10, 10], [2, 3]]
        nc_mat[1:-2, 0:-3] = [[10, 10], [2, 3]]
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))
        dp_mat[2][3] = 100
        nc_mat[2][3] = 100
        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))


