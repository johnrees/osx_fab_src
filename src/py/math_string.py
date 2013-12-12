class MString:
    def __init__(self, string):
        if string.__class__ is MString:
            self.s = string.s
        else:
            self.s = string
    
    def __repr__(self):
        return 'MString(%s)' % self.s
    
    def __str__(self):
        return self.s
        
    def __eq__(self, other):
        if other.__class__ is not MString:
            raise TypeError('Expected MString, got object of %s'
                             % type(other))
        return self.s == other.s
    
    def __neq__(self, other):
        return not self.__eq__(other)
    
    def __len__(self):
        return len(self.s)
    
    def __hash__(self):
        return self.s.__hash__()
    
    def __or__(self, other):
        return MString("(%s) || (%s)" % (self.s, other))
    
    def __ror__(self, other):
        return MString("(%s) || (%s)" % (other, self.s))
        
    def __and__(self, other):
        return MString("(%s) && (%s)" % (self.s, other))

    def __rand__(self, other):
        return MString("(%s) && (%s)" % (other, self.s))
        
    def __invert__(self):
        return MString("!(%s)" % (self.s))
        
    def __add__(self, other):
        return self | other

    def __radd__(self, other):
        return other | self

    def __sub__(self, other):
        return self & ~MString(other)
    
    def __rsub__(self, other):
        return other & ~MString(self)
    
    def replace(self, target, substitution):
        return MString(self.s.replace(target, substitution))